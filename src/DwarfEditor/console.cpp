#include "console.h"

#include <vector>

namespace ot::dedit::console
{
	namespace
	{
		struct console_data
		{
			std::vector<log_data> logs;
		};

		alignas(console_data) char log_storage[sizeof(console_data)];
		static console_data& access_console_data() { return *reinterpret_cast<console_data*>(log_storage); }
	}
	

	void initialize()
	{
		new(log_storage) console_data;
	}

	void clear()
	{
		console_data& data = access_console_data();
		data.logs.clear();
	}

	void output(level_type level, std::string&& s)
	{
		log_data& data = access_console_data().logs.emplace_back();
		data.level = level;
		data.message = std::move(s);
	}

	std::span<log_data const> get_logs()
	{
		return access_console_data().logs;
	}
}
