#include "m3/formula.h"

namespace ot::wf::m3
{
	float get_perceived_threat(character_attributes const& perceiver, int perceiver_count, character_attributes const& perceived)
	{
		float threat_value = 0.0f;

		auto const apply_count_factor_impl = (perceiver_count == 0 || perceiver_count == 1) ? +[](int, float) { return 0.0f; } : +[](int perceiver_count, float factor_value)
		{
			return (perceiver_count - 1) * factor_value;
		};
		auto const apply_count_factor = [perceiver_count, apply_count_factor_impl](float factor_value)
		{
			return apply_count_factor_impl(perceiver_count, factor_value);
		};

		// A high agility target can mask its physical threat from the perceiver
		const float agility_diff_factor = 0.05f;
		const float perceiver_agility = perceiver.agility * (1.0f + apply_count_factor(0.03f));
		const float agility_diff = perceived.agility - perceiver_agility;

		if (agility_diff > 0.0f)
		{
			// TODO
		}

		return threat_value;
	}
}
