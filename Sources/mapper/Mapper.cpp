#include "Mapper.hpp"

void Mapper::load (const ProgramROM& ines)
{
	_impl = ([&] () -> _Impl_type
	{
		switch (ines.idMapper ())
		{
		case 2:
		case 94:
		case 180:
			return MapperUxROM (ines);
		case 0:
		default:
			return MapperNROM (ines);
		}
	})();
}
