#pragma once

#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <utils/NonCopyable.hpp>
#include <utils/NonMovable.hpp>
#include <input/InputGameController.hpp>
#include <core/CoreConfig.hpp>
#include <core/AudioVideoFrame.hpp>

#include <vector>
#include <string>
#include <mutex>
#include <optional>
#include <atomic>

struct Backend;

struct Frontend
: NonCopyable, NonMovable
{

	Frontend (const Options& options);
	~Frontend ();

	auto handle () const -> SDL_Window*;
	auto mainthread() -> int;
	auto pushFrame(Backend&, const AudioVideoFrame&) -> bool;

private:
	void consume ();
	int dispatch (const SDL_Event& ev);

private:
	SDL_Window* _window{nullptr};
	const AudioVideoFrame* _frame{nullptr};
	std::mutex _lockFrame;
	std::optional<InputGameController> _gctrl0;
	std::optional<InputGameController> _gctrl1;
	std::atomic<byte> _inpst0{0};
	std::atomic<byte> _inpst1{0};
};
