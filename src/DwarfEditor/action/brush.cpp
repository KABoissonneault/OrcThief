#include "action/brush.h"

#include "core/fwd_delete.h"

#include <cstdio>

namespace ot::dedit::action
{
	brush_base::~brush_base()
	{

	}

	single_brush::single_brush(brush const& b)
		: id(b.get_id())
		, previous_state(b.mesh_def)
	{

	}

	void single_brush::undo(map& current_map)
	{
		brush* b = current_map.find_brush(id);
		if (b == nullptr)
		{
			std::fprintf(stderr, "error: could not undo single brush action, entity id '%u' not found\n", static_cast<uint32_t>(id));
			return;
		}

		b->reload_node(std::move(previous_state));
	}

	split_edge::split_edge(brush const& b, egfx::half_edge::id edge, math::point3f point)
		: single_brush(b)
		, edge(edge)
		, point(point)
	{

	}

	void split_edge::apply(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			std::fprintf(stderr, "error: could not apply split edge action, entity id '%u' not found\n", static_cast<uint32_t>(get_id()));
			return;
		}

		auto new_mesh = std::make_shared<egfx::mesh_definition>(*b->mesh_def);
		new_mesh->get_half_edge(edge).split_at(point);
		b->reload_node(std::move(new_mesh));
	}
}

template struct ot::fwd_delete<ot::dedit::action::brush_base>;
