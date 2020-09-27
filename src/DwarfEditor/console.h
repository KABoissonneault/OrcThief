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

	void output(level_type level, std::string_view s);
	inline void log(std::string_view s) { output(level_type::log, s); }
	inline void warning(std::string_view s) { output(level_type::warning, s); }
	inline void error(std::string_view s) { output(level_type::error, s); }

	std::span<log_data const> get_logs();
}
