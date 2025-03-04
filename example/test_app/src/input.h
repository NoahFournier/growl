#pragma once

#include "growl/core/api/system_api.h"
#include "growl/core/input/event.h"
#include "growl/core/input/processor.h"

namespace Growl {

class InputHandler : public InputProcessor {
public:
	explicit InputHandler(SystemAPI& system)
		: system{system} {}

	void onMouseEvent(InputMouseEvent& event) override;

	void onKeyboardEvent(InputKeyboardEvent& event) override;

	void onControllerEvent(InputControllerEvent& event) override;

	void onTouchEvent(InputTouchEvent& event) override;

	int getMouseX() {
		return mouseX;
	}

	int getMouseY() {
		return mouseY;
	}

	bool upPressed() {
		return up;
	}

	bool downPressed() {
		return down;
	}

	bool leftPressed() {
		return left;
	}

	bool rightPressed() {
		return right;
	}

private:
	SystemAPI& system;
	int mouseX;
	int mouseY;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};

} // namespace Growl
