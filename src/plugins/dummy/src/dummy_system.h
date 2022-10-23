#pragma once
#include "growl/core/api/api.h"
#include "growl/core/assets/file.h"

namespace Growl {

class DummySystemAPI : public SystemAPIInternal {
public:
	explicit DummySystemAPI(API& api)
		: api{api} {}
	Error init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override {
		return true;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;

	Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start, size_t end) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;

	API& api;
};
} // namespace Growl
