#pragma once

#include <growl/core/game/game.h>
#include <growl/core/graphics/window.h>
#include <memory>

namespace Growl {

class Game;

class GameAdapter {
public:
	explicit GameAdapter(
		std::unique_ptr<Game> game,
		std::unique_ptr<WindowConfig> windowwConfig);
	~GameAdapter();
	void run();

private:
	std::unique_ptr<API> m_api;
	std::unique_ptr<Game> m_game;
	std::unique_ptr<WindowConfig> m_window_config;
};

} // namespace Growl
