#include "serialize_map.h"

#include "core/directive.h"

#include <cstdio>

namespace ot::dedit::serialize
{
	bool fwrite(map_entity const& e, std::FILE* f)
	{
		entity_type_t const type = e.get_type();
		if (!::fwrite(&type, sizeof(type), 1, f))
			return false;

		if (!e.fwrite(f))
			return false;

		auto const children = e.get_children();
		size_t const size = children.size();
		if (!::fwrite(&size, sizeof(size), 1, f))
			return false;

		for (map_entity const& child : children)
		{
			if (!fwrite(child, f))
				return false;
		}

		return true;
	}

	bool fwrite(map const& m, std::FILE* f)
	{
		size_t const version = 1;
		if (!::fwrite(&version, sizeof(version), 1, f))
			return false;

		auto const root_entities = m.get_root_entities();
		size_t const root_entity_count = root_entities.size();

		if (!::fwrite(&root_entity_count, sizeof(root_entity_count), 1, f))
			return false;

		for (map_entity const& e : root_entities)
		{
			if (!fwrite(e, f))
				return false;			
		}

		return true;
	}

	bool do_entity_fread(map& m, root_entity& root_entity, map_entity& parent, std::FILE* f);

	bool fread(map& m, map_entity& e, root_entity& root, std::FILE* f)
	{
		root_entity& child_root = e.get_type() == entity_type_t::root ? static_cast<root_entity&>(e) : root;

		size_t child_count;
		if (!::fread(&child_count, sizeof(child_count), 1, f))
			return false;

		for (size_t n = 0; n < child_count; ++n)
		{
			if (!do_entity_fread(m, child_root, e, f))
				return false;
		}

		return true;
	}
	
	bool do_entity_fread(map& m, root_entity& root_entity, map_entity& parent, std::FILE* f)
	{
		entity_type_t type;
		if (!::fread(&type, sizeof(type), 1, f))
			return false;

		map_entity* current_entity = nullptr;
		switch (type)
		{
		case entity_type_t::root:
			current_entity = &root_entity;
			break;

		case entity_type_t::brush:
		{
			brush& b = m.make_default_brush(m.allocate_entity_id());
			if (!b.fread(parent, f))
				return false;

			current_entity = &b;
			break;
		}

		default:
			return false;
		}

		if (current_entity == nullptr)
			return false;

		if (!fread(m, *current_entity, root_entity, f))
			return false;

		return true;
	}

	bool fread(map& m, std::FILE* f)
	{
		size_t version;
		if (!::fread(&version, sizeof(version), 1, f))
			return false;

		if (version != 1)
			return false;

		size_t entity_count;
		if (!::fread(&entity_count, sizeof(entity_count), 1, f))
			return false;

		root_entity& root = m.get_root();
		for (size_t n = 0; n < entity_count; ++n)
		{
			if (!do_entity_fread(m, root, root, f))
				return false;
		}

		return true;
	}
}
