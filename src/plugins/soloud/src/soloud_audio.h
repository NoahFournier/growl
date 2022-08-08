#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/assets/audio.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/error.h"
#include "soloud.h"
#include "soloud_wav.h"
#include <memory>
namespace Growl {

class SoLoudClip : public Clip {
	friend class SoLoudAudioAPI;

public:
	SoLoudClip(
		std::string name, std::unique_ptr<SoLoud::Wav> wav,
		std::vector<unsigned char>&& raw)
		: Clip(name)
		, sample{std::move(wav)}
		, raw{std::move(raw)} {}

private:
	std::unique_ptr<SoLoud::Wav> sample;
	std::vector<unsigned char> raw;
};

class SoLoudAudioAPI : public AudioAPIInternal {
public:
	explicit SoLoudAudioAPI(SystemAPI& system)
		: system{system} {}
	Error init() override;
	void dispose() override;

	const std::vector<AudioDevice>& getDevices() override {
		return devices;
	}

	Result<std::unique_ptr<Clip>>
	loadSFXFromBundle(AssetsBundle& bundle, std::string path) override;

	void play(Clip& sfx) override;

private:
	SystemAPI& system;
	std::unique_ptr<SoLoud::Soloud> soloud;
	std::vector<AudioDevice> devices;
};

}; // namespace Growl
