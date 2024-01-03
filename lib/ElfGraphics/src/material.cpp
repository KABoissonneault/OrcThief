#include "material.h"

#include "Ogre/Root.h"
#include "Ogre/HlmsManager.h"

namespace ot::egfx
{
	static_assert(sizeof(material_handle_t) == sizeof(Ogre::IdString));
	static_assert(alignof(material_handle_t) == alignof(Ogre::IdString));

	material_handle_t to_material_handle(Ogre::IdString const& s)
	{
		material_handle_t result;
		memcpy(&result, &s, sizeof(material_handle_t));
		return result;
	}

	Ogre::IdString to_id_string(material_handle_t const& h)
	{
		Ogre::IdString result;
		memcpy(&result, &h, sizeof(material_handle_t));
		return result;
	}

	bool material_handle_t::is_null() const
	{
		Ogre::IdString const id_str = to_id_string(*this);
		return id_str == Ogre::IdString();
	}

	bool material_handle_t::operator==(material_handle_t const& rhs) const
	{
		return to_id_string(*this) == to_id_string(rhs);
	}

	std::string material_handle_t::to_debug_string() const
	{
		return to_id_string(*this).getFriendlyText();
	}

	std::vector<material_handle_t> get_registered_materials()
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager* const hlms_manager = root.getHlmsManager();

		std::vector<material_handle_t> result;

		Ogre::HlmsManager::HlmsDatablockMap const& datablocks = hlms_manager->getDatablocks();

		result.reserve(datablocks.size());
		for (auto const& [key, _] : datablocks)
		{
			result.push_back(to_material_handle(key));
		}
		return result;
	}

	std::string get_material_name(material_handle_t const& h)
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager* const hlms_manager = root.getHlmsManager();
		Ogre::HlmsDatablock* const datablock = hlms_manager->getDatablockNoDefault(to_id_string(h));
		if (datablock == nullptr)
			return "<Missing>";

		if (datablock == hlms_manager->getDefaultDatablock())
			return "Default";

		std::string const* name_str = datablock->getNameStr();
		return name_str != nullptr ? *name_str : "<Invalid>";
	}
}
