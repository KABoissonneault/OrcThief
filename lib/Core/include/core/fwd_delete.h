#pragma once

#include "core/fwd_delete.fwd.h"

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