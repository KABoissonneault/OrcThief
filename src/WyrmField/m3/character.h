#pragma once

// M3: Mind, Mettle, and Might
// The three spheres of the "Mind, Spirit, Body" trialism.
// The body represents the material aspects of an entity, anything observable and actionable in the physical world
// The mind represents the knowledge and beliefs of an entity. 
// The spirit represents the relational aspects of an entity, both internal (relationship to self) and external (relationship to others)
namespace ot::wf::m3
{
	// Character attributes are a general score for a character's proficiency in the three spheres
	// Each sphere is judged in three ways:
	// - Output capacity,
	// - Intake capacity, and
	// - Control
	// For example, a Mighty character needs Strength for their might to affect changes upon the world,
	// constitution to survive the attacks of their body, and agility to control how might is applied
	// A strong, low agility character may move large rocks, but may not have the means to hit an agile opponent
	struct character_attributes
	{
		// Mind
		int cognition;
		int cleverness;
		int focus;

		// Mettle
		int charisma;
		int will;
		int wisdom;

		// Might
		int strength;
		int constitution;
		int agility;
	};

	struct enemy_template
	{
		std::string name;
		character_attributes attributes;
	};
}
