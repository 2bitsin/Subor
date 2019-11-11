#pragma once

#include <vector>
#include <array>

#include <utils/SDL2.hpp>
#include <utils/NonCopyable.hpp>
#include <utils/NonMovable.hpp>
#include <input/InputFrame.hpp>

struct InputGameController
: NonCopyable, NonMovable
{
	InputGameController();
	InputGameController(SDL_JoystickID);
	~InputGameController ();

	auto read () const->InputFrame;
	bool read (InputFrame::Button button) const;

	bool update (const SDL_Event& event);

protected:
	using MappingRange = std::pair<const SDL_GameControllerButton*, const SDL_GameControllerButton*>;
	auto _map(InputFrame::Button button) const -> const MappingRange&;

private:
	SDL_JoystickID _id{-1};
	SDL_GameController* _handle{nullptr};

	std::vector<SDL_GameControllerButton> _mappingValues;
	std::array<MappingRange, InputFrame::kNumButtons> _mappingRange;
};