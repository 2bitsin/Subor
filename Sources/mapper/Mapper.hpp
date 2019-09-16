#pragma once


#include "MapperNROM.hpp"
#include "MapperUxROM.hpp"

#include <variant>

struct Mapper
{
	void load (const ProgramROM& ines);

	template <BusOperation _Operation, typename _Host, typename _Data>
	void tick (_Host&& host, word addr, _Data&& data)
	{
		std::visit ([&] (auto&& _impl)
		{
			_impl.tick<_Operation> (host, addr, data);
		}, _impl);
	}

	template <BusOperation _Operation, typename _Host, typename _Data>
	void ppuTick (_Host&& host, word addr, _Data&& data)
	{
		std::visit ([&] (auto&& _impl)
		{
			_impl.ppuTick<_Operation> (host, addr, data);
		}, _impl);
	}

	word ppuMirror (word addr) const
	{
		return std::visit ([&] (auto&& _impl)
		{
			return _impl.ppuMirror (addr);
		}, _impl);
	}

	template <ResetType _Type>
	void reset ()
	{
		std::visit ([&] (auto&& _impl)
		{
			_impl.reset<_Type> ();
		}, _impl);
	}

	Mapper ()
	: _impl{MapperNULL{}}
	{}

private:

	struct MapperNULL
	: MapperBase<MapperNULL>
	{};

	using _Impl_type = std::variant
	<	MapperNULL,
		MapperNROM,
		MapperUxROM>;

	_Impl_type _impl;
};