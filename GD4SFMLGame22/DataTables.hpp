#pragma once
#include <functional>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"

class Aircraft;

struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance)
	{
	}
	float m_angle;
	float m_distance;
};

struct AircraftData
{
	int m_hitpoints;
	float m_speed;
	Textures m_texture;
	sf::IntRect m_walk_texture_rect;
	sf::IntRect m_throw_texture_rect;

	sf::Time m_fire_interval;
	std::vector<Direction> m_directions;
	bool m_has_roll_animation;
	int m_throw_animation_frames;
	int m_walk_animation_frames;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	Textures m_texture;
	sf::IntRect m_texture_rect;
};

struct PickupData
{
	std::function<void(Aircraft&)> m_action;
	Textures m_texture;
	sf::IntRect m_texture_rect;
};

struct ParticleData
{
	sf::Color						m_color;
	sf::Time						m_lifetime;
};

std::vector<AircraftData> InitializeAircraftData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();
std::vector<ParticleData> InitializeParticleData();


