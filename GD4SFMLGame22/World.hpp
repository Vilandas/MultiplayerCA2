#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
#include "AircraftType.hpp"
#include "NetworkNode.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <array>
#include <iostream>
#include <limits>

#include "BloomEffect.hpp"
#include "CommandQueue.hpp"
#include "SoundPlayer.hpp"

#include "NetworkProtocol.hpp"
#include "ParticleNode.hpp"
#include "ParticleType.hpp"
#include "PlayerAction.hpp"
#include "PickupType.hpp"
#include "Pickup.hpp"
#include "PostEffect.hpp"
#include "Projectile.hpp"
#include "SoundNode.hpp"
#include "Utility.hpp"

namespace sf
{
	class RenderTarget;
}



class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked=false);
	void Update(sf::Time dt);
	void Draw();

	sf::FloatRect GetViewBounds() const;
	CommandQueue& GetCommandQueue();

	Aircraft* AddAircraft(int identifier, bool team);
	void RemoveAircraft(int identifier);
	void SetCurrentBattleFieldPosition(float line_y);
	void SetWorldHeight(float height);

	void AddEnemy(AircraftType type, float rel_x, float rel_y);
	void SortEnemies();

	bool HasAlivePlayer() const;
	bool HasPlayerReachedEnd() const;

	void SetWorldScrollCompensation(float compensation);
	Aircraft* GetAircraft(int identifier) const;
	sf::FloatRect GetBattlefieldBounds() const;
	void CreatePickup(sf::Vector2f position, PickupType type);

	void CreatePickup(sf::Vector2f position, PickupType type, int index);
	bool PollGameAction(GameActions::Action& out);

	void StartGame();
	bool HasGameStarted();

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();

	
	void SpawnEnemies();
	void AddEnemies();
	void AddBalls();
	void RespawnBalls(int index);
	void GuideMissiles();
	void HandleCollisions();
	void DestroyEntitiesOutsideView();
	void UpdateSounds();
	void CheckRespawn();

private:
	struct SpawnPoint
	{
		SpawnPoint(AircraftType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{
			
		}
		AircraftType m_type;
		float m_x;
		float m_y;
	};



private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scrollspeed;
	float m_scrollspeed_compensation;
	std::vector<Aircraft*> m_player_aircraft;
	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Aircraft*>	m_active_enemies;

	std::vector<SpawnPoint> m_ball_spawn_points;

	BloomEffect m_bloom_effect;
	bool m_networked_world;
	NetworkNode* m_network_node;
	SpriteNode* m_finish_sprite;

	sf::Vector2f m_position1;
	sf::Vector2f m_position2;
	sf::Vector2f m_position3;
	sf::Vector2f m_position4;
	sf::Vector2f m_position5;

	std::queue<int> m_PickupQueue;

	bool m_game_started;

	//sf::Clock startTimer;
};

