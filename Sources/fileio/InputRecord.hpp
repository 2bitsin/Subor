#pragma once

#include <input/InputPort.hpp>

#include <string>
#include <fstream>
#include <array>

struct InputRecordBase
{
#pragma pack(push, 1)
	struct record_type
	{
		qword count_frames {0};
		qword count_clocks {0};
		byte ports[4u] = {0, 0, 0, 0};
	};
#pragma pack(pop)
	static auto to_array(const byte (&ports) [4u])
		-> std::array<byte, 4u>
	{
		return {
			ports[0u],
			ports[1u],
			ports[2u],
			ports[3u]
		};	
	}
};

struct InputRecordWritter
:	InputRecordBase
{
	InputRecordWritter(const std::string& path)
	:	wfs_(path, std::ios::binary)
	{}

	void push(const InputRecordBase::record_type& r)
	{
		wfs_.write((const char*)&r, sizeof(r));
	};

private:
	std::ofstream wfs_;
};

struct InputRecordReader
:	InputRecordBase
{
	InputRecordReader(const std::string& path)
	:	rfs_(path, std::ios::binary)
	{}

	auto tell()
	{
		return rfs_.tellg()/sizeof(record_type);
	}

	auto seek(std::size_t ofs)
	{
		rfs_.seekg(ofs*sizeof(record_type), std::ios::beg);
	}

	auto seek_rel(std::intptr_t ofs)
	{	
		rfs_.seekg(ofs*sizeof(record_type), std::ios::cur);
	}

	auto pull(InputRecordBase::record_type& r)
	{
		rfs_.read((char*)&r, sizeof(r));
		if (rfs_.gcount() != sizeof(r))
			throw std::runtime_error("Failed reading input frame.");
	}

private:
	std::ifstream rfs_;
};
