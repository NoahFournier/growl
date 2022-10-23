#include "dummy_audio.h"
#include "dummy_graphics.h"
#include "dummy_system.h"
#include "growl/core/api/api.h"
#include <memory>

using Growl::API;
using Growl::DummyAudioAPI;
using Growl::DummyGraphicsAPI;
using Growl::DummySystemAPI;

void initDummyPlugin(API& api) {
	api.addSystemAPI(std::make_unique<DummySystemAPI>(api));
	api.addGraphicsAPI(std::make_unique<DummyGraphicsAPI>(api));
	api.addAudioAPI(std::make_unique<DummyAudioAPI>(api));
}
