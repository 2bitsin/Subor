#include "InputGameController.hpp"

InputGameController::InputGameController ()
:	InputGameController(0)
{}

InputGameController::InputGameController(SDL_JoystickID id)
:	_id {id},
	_handle	{ SDL_GameControllerOpen(id) },
	_mappingValues
	{
		SDL_CONTROLLER_BUTTON_A, 
		SDL_CONTROLLER_BUTTON_Y,

		SDL_CONTROLLER_BUTTON_B, 
		SDL_CONTROLLER_BUTTON_X,

		SDL_CONTROLLER_BUTTON_BACK,
		SDL_CONTROLLER_BUTTON_START,
		SDL_CONTROLLER_BUTTON_DPAD_UP,
		SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		SDL_CONTROLLER_BUTTON_DPAD_RIGHT		
	},
	_mappingRange
	{
		MappingRange{&_mappingValues[0], &_mappingValues[0]+2},
		MappingRange{&_mappingValues[2], &_mappingValues[2]+2},
		MappingRange{&_mappingValues[4], &_mappingValues[4]+1},
		MappingRange{&_mappingValues[5], &_mappingValues[5]+1},
		MappingRange{&_mappingValues[6], &_mappingValues[6]+1},
		MappingRange{&_mappingValues[7], &_mappingValues[7]+1},
		MappingRange{&_mappingValues[8], &_mappingValues[8]+1},
		MappingRange{&_mappingValues[9], &_mappingValues[9]+1}
	}
{
}

InputGameController::~InputGameController ()
{
	if (_handle != nullptr)
	{
		SDL_GameControllerClose (_handle);
	}
}

auto InputGameController::read () const -> InputFrame
{
	InputFrame result;
	if (_handle == nullptr)
		return result;
	result.a = read (InputFrame::Button::A);
	result.b = read (InputFrame::Button::B);
	result.select = read (InputFrame::Button::Select);
	result.start = read (InputFrame::Button::Start);
	result.up = read (InputFrame::Button::Up);
	result.down = read (InputFrame::Button::Down);
	result.left = read (InputFrame::Button::Left);
	result.right = read (InputFrame::Button::Right);
	return result;
}

bool InputGameController::read (InputFrame::Button button) const
{
	if (_handle == nullptr)
		return false;
	auto mapRange = _map (button);
	auto result = false;
	for (auto pMapping = mapRange.first; pMapping != mapRange.second; ++pMapping)
		result = result || SDL_GameControllerGetButton (_handle, *pMapping);
	return result;
}

bool InputGameController::update (const SDL_Event& event)
{
	switch(event.type)
	{
	case SDL_CONTROLLERAXISMOTION:
		if (event.caxis.which == _id)
			return true;
		break;
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		if (event.cbutton.which == _id)
			return true;
		break;
	}
	return false;
}

inline auto InputGameController::_map (InputFrame::Button button) const -> const MappingRange&
{
	return _mappingRange [button];
}
