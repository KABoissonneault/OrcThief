#pragma once

#include "map.h"

namespace ot::dedit
{
	template<typename Application>
	class map_handler
	{
		using derived = Application;

		enum class state
		{
			none,
			confirming_for_new_map,
			confirming_for_open_map,
			confirming_for_quit,
			saving_for_new_map,
			saving_for_open_map,
			saving_for_quit,
		};

		std::string map_path;
		int saved_action = 0;
		state current_state;

		void do_new_map();
		void do_open_map();
		void do_post_save_operation();

		void draw_confirmation_dialog();

	public:
		void update();

		void new_map();
		void open_map();
		void save_map();
		void save_map_as();
		void quit();

		[[nodiscard]] bool can_quit() const noexcept { return current_state == state::none; }

		[[nodiscard]] bool has_map_file() const noexcept { return !map_path.empty(); }
		[[nodiscard]] std::string_view get_map_file() const noexcept { return map_path; }
		
		// Whether the map has been saved since the last applied action
		[[nodiscard]] bool is_map_dirty() const noexcept;
	};

	extern template class map_handler<class application>;
}