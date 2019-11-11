#pragma once

struct NonMovable
{
	NonMovable() = default;
	NonMovable(NonMovable&&) = delete;
	NonMovable& operator = (NonMovable&&) = delete;
};