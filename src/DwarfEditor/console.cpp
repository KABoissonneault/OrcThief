#include "console.h"

#include <vector>

namespace ot::dedit::console
{
	alignas(std::vector<log_data>) char log_storage[sizeof(std::vector<log_data>)];
	static std::vector<log_data>& access_logs() { return *reinterpret_cast<std::vector<log_data>*>(log_storage); }

	void initialize()
	{
		new(log_storage) std::vector<log_data>;
	}

	void clear()
	{
		access_logs().clear();
	}

	void output(level_type level, std::string&& s)
	{
		log_data& data = access_logs().emplace_back();
		data.level = level;
		data.message = std::move(s);
	}

	std::span<log_data const> get_logs()
	{
		return access_logs();
	}
}
