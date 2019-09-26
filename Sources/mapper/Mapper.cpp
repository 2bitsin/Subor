#include "Mapper.hpp"
#include <stdexcept>

void Mapper::load (const ProgramROM& ines)
{
	_impl = ([&] () -> _Impl_type
	{
		switch (ines.idMapper ())
		{
		//case 1:
		//	return MapperMMC1 (ines);
		case 0:
			return MapperNROM (ines);
		case 2:
		case 94:
		case 180:
			return MapperUxROM (ines);
		default:
			throw std::runtime_error("Mapper unsupported!");
		}
	})();
}
