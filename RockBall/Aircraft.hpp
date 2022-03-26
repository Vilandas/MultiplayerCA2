#pragma once
#include "Entity.hpp"
#include "AircraftType.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include "Animation.hpp"
#include "CommandQueue.hpp"
#include "ProjectileType.hpp"
#include "TextNode.hpp"


class Aircraft : public Entity
{
public:
	Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;

	void DisablePickups();
	int GetIdentifier();
	void SetIdentifier(int identifier);
	int GetMissileAmmo() const;
	void SetMissileAmmo(int ammo);

	void IncreaseFireRate();
	void IncreaseSpread();
	void CollectMissiles(unsigned int count);
	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);
	float GetMaxSpeed() const;
	void Fire();
	void LaunchMissile();
	void CreateBullets(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset, const TextureHolder& textures) const;

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void Remove() override;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);


private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);
	bool IsAllied() const;
	void CreatePickup(SceneNode& node, const TextureHolder& textures) const;
	void CheckPickupDrop(CommandQueue& commands);
	void UpdateRollAnimation();


private:
	AircraftType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	Command m_fire_command;
	Command m_missile_command;
	Command m_drop_pickup_command;
	

	bool m_is_firing;
	bool m_is_launching_missile;

	sf::Time m_fire_countdown;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_explosion_began;
	bool m_spawned_pickup;
	bool m_pickups_enabled;


	unsigned int m_fire_rate;
	unsigned int m_spread_level;
	unsigned int m_missile_ammo;
	TextNode* m_health_display;
	TextNode* m_missile_display;
	float m_travelled_distance;
	int m_directions_index;

	int m_identifier;
};

