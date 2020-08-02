#include "map.h"

namespace ot
{
	void map::add_brush(brush b)
	{
		brushes.push_back(std::move(b));
	}
}