#include "serialize_map.h"

#include <cstdio>

namespace ot::dedit::serialize
{
	bool fwrite(map_entity const& e, std::FILE* f)
	{
		entity_id const id = e.get_id();
		if (!::fwrite(&id, sizeof(id), 1, f))
			return false;

		entity_type const type = e.get_type();
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
	
	bool fread(map& m, map_entity& parent, std::FILE* f, map_entity** new_entity)
	{
		entity_id id;
		if (!::fread(&id, sizeof(id), 1, f))
			return false;

		entity_type type;
		if (!::fread(&type, sizeof(type), 1, f))
			return false;

		map_entity* current_entity = nullptr;
		switch (type)
		{
		case entity_type::root:
			current_entity = &m.get_root();
			break;

		default:
			map_entity& e = m.make_default_entity(type, id);
			if (!e.fread(parent, f))
				return false;

			current_entity = &e;
			break;
		}

		if (current_entity == nullptr)
			return false;

		if (new_entity != nullptr)
			*new_entity = current_entity;

		size_t child_count;
		if (!::fread(&child_count, sizeof(child_count), 1, f))
			return false;

		for (size_t n = 0; n < child_count; ++n)
		{
			if (!fread(m, *current_entity, f))
				return false;
		}

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
			if (!fread(m, root, f))
				return false;
		}

		return true;
	}
}
