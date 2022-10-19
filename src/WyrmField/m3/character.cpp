#include "m3/character.h"

namespace ot::wf::m3
{
	character_vitals generate_initial_vitals(character_attributes const& attributes)
	{
		character_vitals vitals;
		vitals.max_energy = attributes.hardiness;
		vitals.current_energy = vitals.max_energy;
		vitals.energy_buffer = 0.0f;
		vitals.max_resolve = attributes.will;
		vitals.current_resolve = vitals.max_resolve;
		vitals.resolve_buffer = 0.0f;
		vitals.max_health = attributes.constitution;
		vitals.current_health = vitals.max_health;
		vitals.health_buffer = 0.0f;
		return vitals;
	}
}
