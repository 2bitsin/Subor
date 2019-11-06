#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <experimental/generator>

#if __has_include("Config.hpp")
	#include "Config.hpp"
#else
	#define DEBUG_DATA_PATH "."s
#endif

struct Options
{
	using cca = const char* const* const;

	enum EmulatorMode
	{
		Normal,
		Playback,
		Record
	};

private:
	
	auto arg_value_or (std::size_t index, std::string defval = "")
	{
		if (index >= _args.size ())
			return defval;
		return _args[index];
	}

	auto env_value_or (std::string index, std::string defval = "")
	{
		if (!_envs.count(index))
			return defval;
		return _envs[index];
	}

	static auto parse_env (cca envp) 
		-> std::unordered_map<std::string, std::string>
	{
		std::unordered_map<std::string, std::string> dest;
		for (auto p = envp; *p; ++p)
		{
			std::string sp = *p;
			auto eq = std::find (sp.begin (), sp.end (), '=');
			if (eq == sp.end ())
				continue;
			std::string k{sp.begin (), eq};
			std::string v{eq + 1, sp.end ()};
			dest.emplace (k, v);
		}
		return dest;
	}
public:
	Options (int argc, cca argv, cca envp)
	: _base{argv [0]},
		_args{argv + 1u, argv + argc},
		_envs{parse_env (envp)}
	{	
		using namespace std::string_literals;
		_game_base_path = DEBUG_DATA_PATH;
		if (_envs.count("SUBOR_DATA_PATH"s))
			_game_base_path = _envs["SUBOR_DATA_PATH"s];		
		if (_game_base_path.back() != '/' 
			&&_game_base_path.back() != '\\')
			_game_base_path += '/';
		
		for (auto i = 0u; i < _args.size(); ++i)
		{
			auto s = _args[i];
			if(s == "--load"s)
			{
				_game_full_path = _game_base_path;
				_game_full_path += arg_value_or(i + 1u, ""s);
				i += 1u;
			}
			else if (s == "--record"s)
				_flag_record = true;
			else if (s == "--playback"s);
				_flag_playback = true;
		}
	}

	auto&& loadrom () const
	{
		return _game_full_path;
	}

	auto mode () const
	{
		if (_flag_playback)
			return EmulatorMode::Playback;
		if (_flag_record)
			return EmulatorMode::Record;
		return EmulatorMode::Normal;
	}

private:
	std::string _base;
	std::vector<std::string> _args;
	std::unordered_map<std::string, std::string> _envs;
	std::string _game_base_path;
	std::string _game_full_path;	
	bool _flag_playback { false };
	bool _flag_record { false };

};