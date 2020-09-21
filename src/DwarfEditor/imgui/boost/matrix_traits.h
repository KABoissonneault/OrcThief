#pragma once

#include "imgui/matrix.h"
#include "imgui/boost/common.h"

#include "core/compiler.h"

OT_IMGUI_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat_traits_defaults.hpp>
#include <boost/qvm/deduce_vec.hpp>
OT_IMGUI_DETAIL_BOOST_INCLUDE_END

template<>
struct boost::qvm::is_mat<ot::dedit::imgui::matrix>
{
	static bool const value = true;
};

template<>
struct boost::qvm::mat_traits<ot::dedit::imgui::matrix> : boost::qvm::mat_traits_defaults<ot::dedit::imgui::matrix, float, 4, 4>
{
	template<int R, int C>
	static inline float& write_element(ot::dedit::imgui::matrix& m)
	{
		static_assert(R < 4 && C < 4, "Out-of-bounds");
		return m.elements[C * 4 + R];
	}

	static inline float& write_element_idx(int r, int c, ot::dedit::imgui::matrix& m)
	{
		assert(r < 4 && c < 4);
		return m.elements[c * 4 + r];
	}
};

