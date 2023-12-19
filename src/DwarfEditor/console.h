#pragma once

#include <string>
#include <string_view>
#include <span>

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

	void output(level_type level, std::string&& s);
		
	inline void log(std::string s)
	{
		output(level_type::log, std::move(s));
	}

	inline void warning(std::string&& s)
	{
		output(level_type::warning, std::move(s));
	}

	inline void error(std::string&& s)
	{
		output(level_type::error, std::move(s));
	}

	std::span<log_data const> get_logs();
}
