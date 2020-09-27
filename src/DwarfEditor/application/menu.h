#pragma once

namespace ot::dedit
{
	class application;

	template<typename Application>
	class menu
	{
		using derived = Application;

	public:
		void update();
	};

	extern template class menu<application>;
}
