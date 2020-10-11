#include <tl/expected.hpp>

namespace ot
{
	template<typename T, typename E>
	using expected = tl::expected<T, E>;
	
	using tl::make_unexpected;
}
