#include "imgui/matrix.h"
#include "imgui/boost/matrix_traits.h"

#include "math/transform_matrix.h"
#include "math/boost/vector_traits.h"
#include "math/boost/transform_matrix_traits.h"
#include "math/boost/transform_matrix_ops.h"
#include "math/boost/quaternion_traits.h"

OT_IMGUI_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/mat.hpp>

#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_vec.hpp>

#include <boost/qvm/quat_operations.hpp>
OT_IMGUI_DETAIL_BOOST_INCLUDE_END

#include <numbers>

#include <im3d_math.h>
#include <im3d_boost_traits.h>

namespace ot::dedit::imgui
{
	matrix matrix::identity() noexcept
	{
		return boost::qvm::identity_mat<float, 4>();
	}

	matrix matrix::from_components(math::vector3f displacement, math::quaternion rotation, math::scales scales) noexcept
	{
		using boost::qvm::operator*=;

		auto m = boost::qvm::convert_to<matrix>(rotation);
		boost::qvm::del_row_col<3, 3>(m) *= boost::qvm::diag_mat(scales);
		boost::qvm::translation(m) = displacement;

		return m;
	}

	math::vector3f matrix::get_displacement() const noexcept
	{
		return boost::qvm::translation(*this);
	}

	math::quaternion matrix::get_rotation() const noexcept
	{
		return ot::math::ops::get_rotation(*this).to_quaternion();
	}

	math::scales matrix::get_scale() const noexcept
	{
		return ot::math::ops::get_scale(*this);
	}

	void matrix::decompose(std::span<float, 3> displacement, std::span<float, 3> euler_rotation, std::span<float, 3> scales) const
	{
		using namespace boost::qvm;

		math::scales const s = get_scale();
		scales[0] = s.x;
		scales[1] = s.y;
		scales[2] = s.z;

		math::rotation_matrix const r = math::ops::get_rotation(*this, s);
		math::euler_rotation const er = r.get_euler_angles_zyx();
		euler_rotation[0] = er.x;
		euler_rotation[1] = er.y;
		euler_rotation[2] = er.z;

		math::vector3f const d = get_displacement();
		displacement[0] = d.x;
		displacement[1] = d.y;
		displacement[2] = d.z;
	}

	static float clamp(float f)
	{
		return std::fmod(f + std::numbers::pi_v<float>, std::numbers::pi_v<float> * 2.f) - std::numbers::pi_v<float>;
	}

	void matrix::recompose(std::span<float const, 3> displacement, std::span<float const, 3> euler_rotation, std::span<float const, 3> scales)
	{
		using namespace boost::qvm;
		using boost::qvm::operator*;

		math::vector3f const d{ displacement[0], displacement[1], displacement[2] };
		auto const r = rot_mat_zyx<3>(
			clamp(euler_rotation[2])
			, clamp(euler_rotation[1])
			, clamp(euler_rotation[0])
			);
		math::scales const s{ scales[0], scales[1], scales[2] };
				
		auto& rot_mat = del_row_col<3, 3>(*this);
		assign(
			rot_mat,
			r * diag_mat(s)
		);
	
		assign(translation(*this), d);
	}

	math::transform_matrix to_math_matrix(matrix const& m)
	{
		return boost::qvm::convert_to<math::transform_matrix>(m);
	}

	matrix invert(matrix const& t)
	{
		return boost::qvm::inverse(t);
	}

	matrix operator*(matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	matrix operator*(math::transform_matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	matrix operator*(matrix const& lhs, math::transform_matrix const& rhs)
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	bool float_eq(matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	
	}
	bool float_eq(math::transform_matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	}

	bool float_eq(matrix const& lhs, math::transform_matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	}

	void text(matrix const& m)
	{
		float displacement[3];
		float rotation[3];
		float scales[3];

		m.decompose(displacement, rotation, scales);

		rotation[0] *= 180.f / std::numbers::pi_v<float>;
		rotation[1] *= 180.f / std::numbers::pi_v<float>;
		rotation[2] *= 180.f / std::numbers::pi_v<float>;

		ImGui::Text("{%f %f %f} {%f %f %f} {%f %f %f}"
			, displacement[0], displacement[1], displacement[2]
			, rotation[0], rotation[1], rotation[2]
			, scales[0], scales[1], scales[2]
		);
	}

	void text(matrix const& m, std::string_view title)
	{
		float displacement[3];
		float rotation[3];
		float scales[3];

		m.decompose(displacement, rotation, scales);

		rotation[0] *= 180.f / std::numbers::pi_v<float>;
		rotation[1] *= 180.f / std::numbers::pi_v<float>;
		rotation[2] *= 180.f / std::numbers::pi_v<float>;

		ImGui::Text("%.*s: {%f %f %f} {%f %f %f} {%f %f %f}"
			, static_cast<int>(title.size()), title.data()
			, displacement[0], displacement[1], displacement[2]
			, rotation[0], rotation[1], rotation[2]
			, scales[0], scales[1], scales[2]
		);
	}
}

namespace ot::math
{
	dedit::imgui::matrix to_imgui(transform_matrix const& m)
	{
		return boost::qvm::convert_to<dedit::imgui::matrix>(m);
	}
}
