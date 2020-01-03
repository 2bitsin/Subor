#pragma once

#include <fileio/InputRecord.hpp>
#include <utils/Options.hpp>

#include <cassert>
#include <variant>
#include <array>

template <typename _InputPort>
struct InputProxyLogger
:	public InputRecordWritter
{
	InputProxyLogger(_InputPort& port, const std::string& path)
	:	InputRecordWritter(path),
		input_port_(port)
	{}

	template <typename _Host>
	auto read(_Host&& host)
		-> std::array<byte, 4u> 
	{
		write_frame(host);			
		return input_port_.read(host);
	}

	template <typename _Host>
	auto write_frame(_Host&& host)
	{
		auto ports = input_port_.read(host);
		last_ = InputRecordBase::record_type
		{
			.count_frames = host.count_frames(),
			.count_clocks = host.count_clocks(),
			.ports = 
			{
				ports[0u], 
				ports[1u], 
				ports[2u], 
				ports[3u]
			}
		};
		return InputRecordWritter::push(last_);	
	}
protected:
	InputRecordBase::record_type last_;
	_InputPort& input_port_;
};

template <typename _InputPort>
struct InputProxyPlayer
:	InputRecordReader
{
	InputProxyPlayer(_InputPort& port, const std::string& path)
	:	InputRecordReader(path), 
		input_port_(port)
	{}

	template <typename _Host>
	auto read(_Host&& host)
		-> std::array<byte, 4u> 
	{
		auto fc = host.count_frames();
		auto cc = host.count_clocks();
		
		auto off = tell();
		InputRecordBase::record_type r;

	load_next_frame:
		pull(r);
		if (r.count_frames > fc)
		{
			seek(off);
			return to_array(last_.ports);
		}

		last_ = r;
		if(r.count_frames < fc)
			goto load_next_frame;

		assert(r.count_frames == fc);
		//assert(r.count_clocks == cc);
		return to_array(last_.ports);
	}

protected:
	_InputPort& input_port_;
	record_type last_;
};

template <typename _InputPort>
struct InputProxyForward
{
	InputProxyForward(_InputPort& port)
	:	input_port_{port}
	{}

	template <typename _Host>
	auto read(_Host&& host)
		-> std::array<byte, 4u> 
	{
		return input_port_.read(host);
	}

protected:
	_InputPort &input_port_;
};


template <typename _InputPort>
struct InputProxy
{
private:
	using impl = std::variant
	<	InputProxyForward<_InputPort>,
		InputProxyLogger<_InputPort>, 
		InputProxyPlayer<_InputPort>>;
public:

	InputProxy(_InputPort& port, const Options& opts)
	:	input_wrpl_{choose_impl(port, opts)}
	{}

	template <typename _Host>
	std::array<byte, 4> read(_Host&& host)
	{
		return std::visit([&] (auto&& pp) {
			return pp.read(host);
		}, input_wrpl_);
	}

private:
	static impl choose_impl(_InputPort& port, const Options& opts)
	{
		const auto path = opts.cmd_load().value_or("default") + ".input";
		switch(opts.mode())
		{
		case Options::EmulatorMode::Playback:
			return InputProxyPlayer<_InputPort>{port, path};
		case Options::EmulatorMode::Record:
			return InputProxyLogger<_InputPort>{port, path};
		default:
			break;
		}	
		return InputProxyForward<_InputPort>{port};
	}
	
	impl input_wrpl_;
};