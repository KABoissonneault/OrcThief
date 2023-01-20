#pragma once

#include "m3/character.h"

namespace ot::wf::m3
{
	// Value above 0 means a threat to perceiver
	// Value below 0 means not a threat
	// More perceivers means less threat from perceived
	float get_perceived_threat(character_attributes const& perceiver, int perceiver_count, character_attributes const& perceived);

	int roll_initiative(character_attributes const& character);
}
