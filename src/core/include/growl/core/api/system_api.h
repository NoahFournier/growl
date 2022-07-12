#pragma once

#include "../log.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "growl/core/input/processor.h"
#include "growl/util/error.h"
#include <memory>
#include <string>
#include <string_view>

namespace Growl {

class Window;
class WindowConfig;

class SystemAPI {
public:
	virtual ~SystemAPI() {}
	virtual bool isRunning() = 0;
	virtual void tick() = 0;
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& window) = 0;
	virtual void setInputProcessor(InputProcessor* processor) {
		inputProcessor = processor;
	}

	template <class... Args>
	void
	log(LogLevel log_level, std::string tag, std::string_view msg,
		Args&&... args) {
		logInternal(
			log_level, tag, fmt::format(msg, std::forward<Args>(args)...));
	}

	template <class... Args>
	void log(std::string tag, std::string_view msg, Args&&... args) {
		log(LogLevel::INFO, tag, msg, args...);
	}

	virtual void setLogLevel(LogLevel log_level) = 0;

protected:
	virtual void
	logInternal(LogLevel log_level, std::string tag, std::string formatted) = 0;
	InputProcessor* inputProcessor;
};

} // namespace Growl
