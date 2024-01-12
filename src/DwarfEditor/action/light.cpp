#include "light.h"

#include "map.h"
#include "console.h"

#include <format>

namespace ot::dedit::action
{
	single_light::single_light(egfx::light_cref l)
		: single_object(l)
	{

	}

	void single_light::do_apply(egfx::object_ref o, bool is_redo)
	{
		do_apply(o.as<egfx::light_ref>(), is_redo);
	}

	void single_light::do_undo(egfx::object_ref o)
	{
		do_undo(o.as<egfx::light_ref>());
	}

	set_light_type::set_light_type(egfx::light_cref l, egfx::light_type new_type)
		: single_light(l)
		, new_value(new_type)
		, old_value(l.get_light_type())
	{

	}

	void set_light_type::do_apply(egfx::light_ref l, bool is_redo)
	{
		(void)is_redo;
		l.set_light_type(new_value);
	}

	void set_light_type::do_undo(egfx::light_ref l)
	{
		l.set_light_type(old_value);
	}

	set_light_power_scale::set_light_power_scale(egfx::light_cref l, float new_value)
		: single_light(l)
		, new_value(new_value)
		, old_value(l.get_power_scale())
	{

	}

	void set_light_power_scale::do_apply(egfx::light_ref l, bool is_redo)
	{
		(void)is_redo;
		l.set_power_scale(new_value);
	}

	void set_light_power_scale::do_undo(egfx::light_ref l)
	{
		l.set_power_scale(old_value);
	}

	set_light_diffuse::set_light_diffuse(egfx::light_cref l, egfx::color new_value)
		: single_light(l)
		, new_value(new_value)
		, old_value(l.get_diffuse())
	{

	}

	void set_light_diffuse::do_apply(egfx::light_ref l, bool is_redo)
	{
		(void)is_redo;
		l.set_diffuse(new_value);
	}

	void set_light_diffuse::do_undo(egfx::light_ref l)
	{
		l.set_diffuse(old_value);
	}

	set_light_attenuation::set_light_attenuation(egfx::light_cref l, float new_range, float new_const, float new_linear, float new_quadratic)
		: single_light(l)
		, new_range(new_range)
		, new_const(new_const)
		, new_linear(new_linear)
		, new_quadratic(new_quadratic)
		, old_range(l.get_attenuation_range())
		, old_const(l.get_attenuation_const())
		, old_linear(l.get_attenuation_linear())
		, old_quadratic(l.get_attenuation_quadratic())
	{

	}

	void set_light_attenuation::do_apply(egfx::light_ref l, bool is_redo)
	{
		(void)is_redo;
		l.set_attenuation(new_range, new_const, new_linear, new_quadratic);
	}

	void set_light_attenuation::do_undo(egfx::light_ref l)
	{
		l.set_attenuation(old_range, old_const, old_linear, old_quadratic);
	}
}
