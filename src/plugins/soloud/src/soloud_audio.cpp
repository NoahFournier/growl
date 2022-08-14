#include "soloud_audio.h"
#include "bundle_file.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/error.h"
#include "growl/core/audio/device.h"
#include "growl/core/error.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include <memory>
#include <string>
#include <vector>

using Growl::AssetsBundle;
using Growl::AudioClip;
using Growl::AudioStream;
using Growl::Error;
using Growl::Result;
using Growl::SoLoudAudioAPI;
using Growl::SoLoudAudioClip;
using Growl::SoLoudAudioStream;

Error SoLoudAudioAPI::init() {
	soloud = std::make_unique<SoLoud::Soloud>();
	soloud->init();
	soloud->setGlobalVolume(1.0);

	this->devices = std::vector<AudioDevice>{
		{soloud->getBackendString(),
		 static_cast<int>(soloud->getBackendSamplerate())}};

	system.log(
		"SoLoudAudioAPI",
		"Initialised SoLoud audio; using [{}] backend at {}Hz",
		devices[0].getName(), devices[0].getSampleRate());

	return nullptr;
}

void SoLoudAudioAPI::dispose() {
	system.log("SoLoudAudioAPI", "Shutting down SoLoud");
	soloud->deinit();
}

Result<std::unique_ptr<AudioClip>>
SoLoudAudioAPI::loadClipFromBundle(AssetsBundle& bundle, std::string path) {
	auto wav = std::make_unique<SoLoud::Wav>();
	auto bundle_result = bundle.getRawData(path);
	if (bundle_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load clip: " + bundle_result.error()->message()));
	}
	auto raw = std::move(bundle_result.get());
	if (int error = wav->loadMem(raw.data(), raw.size(), false, false); error) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load clip: got SoLoud error code " +
			std::to_string(error)));
	}
	std::unique_ptr<AudioClip> ret(std::make_unique<SoLoudAudioClip>(
		path, std::move(wav), std::move(raw)));
	return std::move(ret);
}

Result<std::unique_ptr<AudioStream>>
SoLoudAudioAPI::createStreamFromBundle(AssetsBundle& bundle, std::string name) {
	Result<std::unique_ptr<SoLoudBundleFile>> bundle_file_result =
		openFileFromBundle(bundle, name);
	if (bundle_file_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load file from bundle: " +
			bundle_file_result.error()->message()));
	}
	auto file = std::move(bundle_file_result.get());
	auto stream = std::make_unique<SoLoud::WavStream>();
	if (int error = stream->loadFile(file.get()); error) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load stream: got SoLoud error code " +
			std::to_string(error)));
	}
	std::unique_ptr<AudioStream> ret(std::make_unique<SoLoudAudioStream>(
		name, std::move(stream), std::move(file)));
	return std::move(ret);
}

void SoLoudAudioAPI::play(AudioClip& clip) {
	auto& soloud_clip = static_cast<SoLoudAudioClip&>(clip);
	soloud->play(*soloud_clip.sample);
}

void SoLoudAudioAPI::play(AudioStream& stream) {
	auto& soloud_stream = static_cast<SoLoudAudioStream&>(stream);
	soloud->play(*soloud_stream.stream);
}
