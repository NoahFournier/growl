#include "opengl_graphics.h"
#include "SDL_video.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/mat4x4.hpp"
#include "opengl.h"
#include "opengl_batch.h"
#include "opengl_texture.h"
#include "opengl_texture_atlas.h"
#include <memory>
#include <vector>

using Growl::Atlas;
using Growl::Batch;
using Growl::Error;
using Growl::FontTextureAtlas;
using Growl::OpenGLGraphicsAPI;
using Growl::Texture;
using Growl::TextureAtlas;
using Growl::TextureOptions;
using std::chrono::duration;
using std::chrono::seconds;

OpenGLGraphicsAPI::OpenGLGraphicsAPI(SystemAPI* system)
	: system{system} {}

Error OpenGLGraphicsAPI::init() {
	last_render = high_resolution_clock::now();
	return nullptr;
}

void OpenGLGraphicsAPI::dispose() {
	SDL_GL_DeleteContext(context);
}

void OpenGLGraphicsAPI::begin() {
	auto tp = high_resolution_clock::now();
	deltaTime = duration<double, seconds::period>(tp - last_render).count();
	last_render = tp;
}

void OpenGLGraphicsAPI::end() {
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(window->getNative()));
}

Error OpenGLGraphicsAPI::setWindow(const WindowConfig& config) {
	auto window_result = system->createWindow(config);
	if (window_result.hasError()) {
		return std::move(window_result.error());
	}
	window = std::move(window_result.get());

#ifdef GROWL_OPENGL_3_3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif GROWL_OPENGL_4_5
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#endif
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	context =
		SDL_GL_CreateContext(static_cast<SDL_Window*>(window->getNative()));
#ifdef GROWL_OPENGL_4_5
	gladLoadGLLoader(SDL_GL_GetProcAddress);
#endif

	system->log(
		"OpenGLGraphicsAPI", "Loaded OpenGL version {}.{}", GLVersion.major,
		GLVersion.minor);

	glViewport(0, 0, config.getWidth(), config.getHeight());
	default_shader = std::make_unique<OpenGLShader>(*this);
	setupDebugCallback();

	return nullptr;
}

void OpenGLGraphicsAPI::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::createTexture(
	const Image& image, const TextureOptions options) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image.getRaw());

	return setupTexture(
		texture_id, image.getWidth(), image.getHeight(), options);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::createTexture(
	unsigned int width, unsigned int height, const TextureOptions options) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	std::vector<unsigned char> data(4 * width * height, 0);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		data.data());

	return setupTexture(texture_id, width, height, options);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::setupTexture(
	unsigned int texture_id, int width, int height,
	const TextureOptions options) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (options.mipmapped) {
		glGenerateMipmap(GL_TEXTURE_2D);
		if (options.filtering) {
			glTexParameteri(
				GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(
				GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	} else if (options.filtering) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return std::make_unique<OpenGLTexture>(texture_id, width, height);
}

std::unique_ptr<TextureAtlas> OpenGLGraphicsAPI::createTextureAtlas(
	const Atlas& atlas, const TextureOptions options) {
	return std::make_unique<OpenGLTextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<FontTextureAtlas> OpenGLGraphicsAPI::createFontTextureAtlas(
	const FontAtlas& atlas, const TextureOptions options) {
	return std::make_unique<FontTextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch() {
	int w, h;
	SDL_GetWindowSize(static_cast<SDL_Window*>(window->getNative()), &w, &h);
	glViewport(0, 0, w, h);
	auto projection = glm::ortho<float>(0, w, h, 0, 1, -1);
	return std::make_unique<OpenGLBatch>(
		default_shader.get(), projection, w, h, 0);
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch(const Texture& texture) {
	auto& opengl_texture = static_cast<const OpenGLTexture&>(texture);
	glViewport(0, 0, texture.getWidth(), texture.getHeight());
	auto projection =
		glm::ortho<float>(0, texture.getWidth(), 0, texture.getHeight(), 1, -1);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		opengl_texture.getRaw(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_unique<OpenGLBatch>(
		default_shader.get(), projection, texture.getWidth(),
		texture.getHeight(), fbo);
}

void OpenGLGraphicsAPI::checkGLError(const char* file, long line) {
	int err = glGetError();
	if (err) {
		system->log(
			LogLevel::ERROR, "OpenGL", "Error {:#04x} at {}:{}", err, file,
			line);
	};
}

void OpenGLGraphicsAPI::checkShaderCompileError(unsigned int shader) {
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int info_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
		char* log = new char[info_len];
		glGetShaderInfoLog(shader, info_len, &info_len, log);
		system->log(
			LogLevel::ERROR, "OpenGL", "Error compiling shader: {}", log);
		delete[] log;
	}
}

void OpenGLGraphicsAPI::setupDebugCallback() {
#ifdef GROWL_OPENGL_4_5
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(
		[](GLenum source, GLenum type, GLuint id, GLenum severity,
		   GLsizei length, const GLchar* message, const void* user_param) {
			reinterpret_cast<const OpenGLGraphicsAPI*>(user_param)
				->onGLDebugMessage(source, type, id, severity, message);
		},
		this);
	checkGLError(__FILE__, __LINE__);
#endif
}

void OpenGLGraphicsAPI::onGLDebugMessage(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	const GLchar* message) const {
	if (severity == GL_DEBUG_SEVERITY_HIGH ||
		severity == GL_DEBUG_SEVERITY_MEDIUM) {
		system->log(LogLevel::ERROR, "OpenGL", "{}", message);
	}
}
