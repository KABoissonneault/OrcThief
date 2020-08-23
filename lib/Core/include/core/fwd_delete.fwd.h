#pragma once

namespace ot
{
	// Struct used for forward declaring a template
	template<typename T>
	struct fwd_delete
	{
		void operator()(T* p) const noexcept;
	};

	template<typename T>
	struct fwd_delete<T[]>
	{
		void operator()(T* p) const noexcept;
	};
}
