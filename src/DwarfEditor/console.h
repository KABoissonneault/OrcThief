#pragma once

#include <string>
#include <string_view>
#include <span>
#include <fmt/format.h>

namespace ot::dedit::console
{
	void initialize();

	void clear();

	enum class level_type
	{
		verbose,
		log,
		warning,
		error,
	};

	struct log_data
	{
		std::string message;
		level_type level;
	};

	void output(level_type level, std::string s);
	
	inline void log(std::string_view s) 
	{ 
		output(level_type::log, std::string(s)); 
	}
	
	template<typename... T>
	inline void log(std::string_view s, T const&... args)
	{
		output(level_type::log, fmt::format(s, args...));
	}

	inline void warning(std::string_view s) 
	{ 
		output(level_type::warning, std::string(s)); 
	}

	template<typename... T>
	inline void warning(std::string_view s, T const&... args)
	{
		output(level_type::warning, fmt::format(s, args...));
	}
	
	inline void error(std::string_view s) 
	{ 
		output(level_type::error, std::string(s)); 
	}

	template<typename... T>
	inline void error(std::string_view s, T const&... args)
	{
		output(level_type::error, fmt::format(s, args...));
	}

	std::span<log_data const> get_logs();
}
