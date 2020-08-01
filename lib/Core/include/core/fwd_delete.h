#pragma once

#include "core/uptr.h"

namespace ot
{
	template<typename T>
	void fwd_delete<T>::operator()(T* p) const noexcept
	{
		delete p;
	}

	template<typename T>
	void fwd_delete<T[]>::operator()(T* p) const noexcept
	{
		delete[] p;
	}
}