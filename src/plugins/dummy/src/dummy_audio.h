#pragma once

#include "growl/core/api/api.h"
#include "growl/core/audio/device.h"
#include <vector>
namespace Growl {

class DummyAudioAPI : public AudioAPIInternal {
public:
	explicit DummyAudioAPI(API& api)
		: api{api} {}
	Error init() override;
	void dispose() override;

	const std::vector<AudioDevice>& getDevices() override {
		return devices;
	}

	Result<std::unique_ptr<AudioClip>>
	loadClipFromBundle(AssetsBundle& bundle, std::string name) override;

	Result<std::unique_ptr<AudioStream>>
	createStreamFromBundle(AssetsBundle& bundle, std::string name) override;

	void play(AudioClip& sfx) override;
	void play(AudioStream& stream, bool loop = true) override;

private:
	API& api;
	std::vector<AudioDevice> devices;
};

} // namespace Growl
