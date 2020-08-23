#pragma once

#include "action/brush.fwd.h"
#include "graphics/mesh_definition.fwd.h"
#include "map.h"

#include <memory>

namespace ot::action
{
	class brush_base
	{
	public:
		virtual ~brush_base();

		virtual void apply(map& current_map) = 0;
		virtual void undo(map& current_map) = 0;
	};

	class single_brush : public brush_base
	{
		entity_id id;
		std::shared_ptr<graphics::mesh_definition const> previous_state;
	protected:
		single_brush(brush const& b);
		
		entity_id get_id() const noexcept { return id; }

	public:
		virtual void undo(map& current_map) override;
	};

	class split_edge : public single_brush
	{
		graphics::half_edge::id edge;
		math::point3d point;

	public:
		split_edge(brush const& b, graphics::half_edge::id edge, math::point3d point);

		virtual void apply(map& current_map) override;
	};
}
