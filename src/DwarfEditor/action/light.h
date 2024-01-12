#pragma once

#include "base.h"
#include "map_entity.h"

#include "egfx/object/light.fwd.h"
#include "egfx/color.h"

#include "map.fwd.h"

namespace ot::dedit::action
{
	class single_light : public single_object
	{
	protected:
		single_light(egfx::light_cref l);
				
		virtual void do_apply(egfx::light_ref l, bool is_redo) = 0;
		virtual void do_undo(egfx::light_ref l) = 0;

		virtual void do_apply(egfx::object_ref o, bool is_redo) override final;
		virtual void do_undo(egfx::object_ref o) override final;
	};

	class set_light_type : public single_light
	{
		egfx::light_type new_value;
		egfx::light_type old_value;

	protected:
		virtual void do_apply(egfx::light_ref l, bool is_redo) override;
		virtual void do_undo(egfx::light_ref l) override;

	public:
		set_light_type(egfx::light_cref l, egfx::light_type new_type);
	};

	class set_light_power_scale : public single_light
	{
		float new_value;
		float old_value;

	protected:
		virtual void do_apply(egfx::light_ref l, bool is_redo) override;
		virtual void do_undo(egfx::light_ref l) override;

	public:
		set_light_power_scale(egfx::light_cref l, float new_scale);
	};

	class set_light_diffuse : public single_light
	{
		egfx::color new_value;
		egfx::color old_value;

	protected:
		virtual void do_apply(egfx::light_ref l, bool is_redo) override;
		virtual void do_undo(egfx::light_ref l) override;

	public:
		set_light_diffuse(egfx::light_cref l, egfx::color new_color);
	};

	class set_light_attenuation : public single_light
	{
		float new_range;
		float new_const;
		float new_linear;
		float new_quadratic;
		float old_range;
		float old_const;
		float old_linear;
		float old_quadratic;

	protected:
		virtual void do_apply(egfx::light_ref l, bool is_redo) override;
		virtual void do_undo(egfx::light_ref l) override;

	public:
		set_light_attenuation(egfx::light_cref l, float new_range, float new_const, float new_linear, float new_quadratic);
	};
}
