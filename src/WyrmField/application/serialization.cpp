#include "application/serialization.h"

#include "m3/character.h"

#include <iostream>
#include <nlohmann/json.hpp>

namespace ot::wf
{
	namespace
	{
		enum class enemy_template_versions : int
		{
			initial = 0
		};

		enemy_template_versions const enemy_template_version = enemy_template_versions::initial;
	}

	void save_enemy_template(std::ostream& o, std::span<m3::enemy_template> t)
	{
		nlohmann::json output_data;
		output_data["version"] = enemy_template_version;
		
		{
			nlohmann::json& t_data = output_data["templates"];

			for (m3::enemy_template const& e : t)
			{
				nlohmann::json e_data;
				e_data["name"] = e.name;
				if(!e.portrait.empty())
					e_data["portrait"] = e.portrait;

				nlohmann::json& attributes = e_data["attributes"];
				attributes.push_back(e.attributes.cleverness);
				attributes.push_back(e.attributes.hardiness);
				attributes.push_back(e.attributes.focus);
				attributes.push_back(e.attributes.charisma);
				attributes.push_back(e.attributes.will);
				attributes.push_back(e.attributes.wisdom);
				attributes.push_back(e.attributes.strength);
				attributes.push_back(e.attributes.constitution);
				attributes.push_back(e.attributes.agility);

				t_data.push_back(e_data);
			}
		}

		o << output_data;
	}

	std::vector<m3::enemy_template> load_enemy_template(std::istream& i)
	{
		std::vector<m3::enemy_template> output;

		nlohmann::json const data = nlohmann::json::parse(i);
		int const parsed_version = data["version"].get<int>();
		if (parsed_version > (int)enemy_template_version)
		{
			throw std::runtime_error("Can't parsed enemy template file: unsupported version '{}'");
		}

		nlohmann::json const& templates_data = data["templates"];
		for (nlohmann::json const& template_data : templates_data)
		{
			m3::enemy_template& e = output.emplace_back();
			e.name = template_data["name"].get<std::string>();
			
			if (template_data.contains("portrait"))
			{
				e.portrait = template_data["portrait"].get<std::string>();
			}

			nlohmann::json const& attributes = template_data["attributes"];
			e.attributes.cleverness = attributes[0];
			e.attributes.hardiness = attributes[1];
			e.attributes.focus = attributes[2];
			e.attributes.charisma = attributes[3];
			e.attributes.will = attributes[4];
			e.attributes.wisdom = attributes[5];
			e.attributes.strength = attributes[6];
			e.attributes.constitution = attributes[7];
			e.attributes.agility = attributes[8];
		}

		return output;
	}
}