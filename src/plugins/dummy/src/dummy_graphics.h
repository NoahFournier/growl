#pragma once

#include "growl/core/api/api.h"

namespace Growl {

class DummyGraphicsAPI : public GraphicsAPIInternal {
public:
	explicit DummyGraphicsAPI(API& api)
		: api{api} {}
	Error init() override;
	void dispose() override;
	void begin() override;
	void end() override;
	Error setWindow(const WindowConfig& window_config) override;
	void clear(float r, float g, float b) override;
	void onWindowResize(int width, int height) override;

	std::unique_ptr<Texture>
	createTexture(const Image& image, const TextureOptions options) override;
	std::unique_ptr<Texture> createTexture(
		unsigned int width, unsigned int height,
		const TextureOptions options) override;

	std::unique_ptr<TextureAtlas> createTextureAtlas(
		const Atlas& atlas, const TextureOptions options) override;

	std::unique_ptr<FontTextureAtlas>
	createFontTextureAtlas(const FontFace& face) override;

	std::unique_ptr<Batch> createBatch() override;
	std::unique_ptr<Batch> createBatch(const Texture& texture) override;

private:
	API& api;
};

} // namespace Growl
