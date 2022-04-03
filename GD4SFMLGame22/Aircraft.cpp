#include "Aircraft.hpp"

#include <iostream>

#include "DataTables.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "Projectile.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "DataTables.hpp"
#include "Pickup.hpp"
#include "PickupType.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"




namespace
{
	const std::vector<AircraftData> Table = InitializeAircraftData();
}


Aircraft::Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[static_cast<int>(type)].m_hitpoints)
, m_type(type)
, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_walk_texture_rect)
, m_explosion(textures.Get(Textures::kExplosion))
, m_is_firing(false)
, m_is_launching_missile(false)
, m_fire_countdown(sf::Time::Zero)
, m_is_marked_for_removal(false)
, m_show_explosion(true)
, m_explosion_began(false)
, m_spawned_pickup(false)
, m_pickups_enabled(true)
, m_fire_rate(1)
, m_spread_level(1)
, m_missile_ammo(2)
, m_has_ball(true)
, m_health_display(nullptr)
, m_missile_display(nullptr)
, m_travelled_distance(0.f)
, m_directions_index(0)
, m_identifier(0)
, m_TeamPink(true)
{
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	
	m_current_walk_frame = 0;
	m_current_shoot_frame = 0;


	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);

	m_fire_command.category = static_cast<int>(Category::Type::kScene);
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreateBullets(node, textures);
	};

	m_missile_command.category = static_cast<int>(Category::Type::kScene);
	m_missile_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreateProjectile(node, ProjectileType::kMissile, 0.f, 0.5f, textures);
	};

	m_drop_pickup_command.category = static_cast<int>(Category::Type::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreatePickup(node, textures);
	};
	
	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	m_health_display = healthDisplay.get();
	AttachChild(std::move(healthDisplay));

	if (Aircraft::GetCategory() == static_cast<int>(Category::kPlayerAircraft))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		m_missile_display = missileDisplay.get();
		AttachChild(std::move(missileDisplay));
	}

	//UpdateTexts();
	if (m_TeamPink) 
	{
		setScale(-3, 3);
	}
	else
	{ 
		setScale(3, 3); 
	}
}

int Aircraft::GetMissileAmmo() const
{
	return m_missile_ammo;
}

void Aircraft::SetMissileAmmo(int ammo)
{
	m_missile_ammo = ammo;
}

void Aircraft::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(IsDestroyed() && m_show_explosion)
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

void Aircraft::DisablePickups()
{
	m_pickups_enabled = false;
}


unsigned int Aircraft::GetCategory() const
{
	if (IsAllied())
	{
		return static_cast<int>(Category::kPlayerAircraft);
	}
	return static_cast<int>(Category::kEnemyAircraft);
}

void Aircraft::IncreaseFireRate()
{
	if(m_fire_rate < 10)
	{
		++m_fire_rate;
	}
}

bool Aircraft::GetTeamPink(){
	return m_TeamPink;
}

void Aircraft::SetTeamPink(bool team) {
	m_TeamPink = team;
}

void Aircraft::IncreaseSpread()
{
	if(m_spread_level < 3)
	{
		++m_spread_level;
	}
}

void Aircraft::CollectMissiles(unsigned int count)
{
	m_missile_ammo += count;
}

void Aircraft::UpdateTexts()
{
	if(IsDestroyed())
	{
		m_health_display->SetString("");
	}
	else
	{
		m_health_display->SetString("^");
	}
	m_health_display->setPosition(0.f, 50.f);
	m_health_display->setRotation(-getRotation());

	if(m_missile_display)
	{
		if(m_missile_ammo == 0)
		{
			m_missile_display->SetString("");
		}
		else
		{
			m_missile_display->SetString("M: " + std::to_string(m_missile_ammo));
		}
	}

}

void Aircraft::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	/*UpdateTexts();*/
	UpdateRollAnimation();

	//Entity has been destroyed, possibly drop pickup, mark for removal
	if(IsDestroyed())
	{
		//CheckPickupDrop(commands);
		m_explosion.Update(dt);

		// Play explosion sound only once
		if (!m_explosion_began)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);

			//Emit network game action for enemy explodes
			if(!IsAllied())
			{
				sf::Vector2f position = GetWorldPosition();

				Command command;
				command.category = Category::kNetwork;
				command.action = DerivedAction<NetworkNode>([position](NetworkNode& node, sf::Time)
				{
					node.NotifyGameAction(GameActions::EnemyExplode, position);
				});

				commands.Push(command);
			}

			m_explosion_began = true;
		}
		return;
	}
	//Check if bullets or missiles are fired
	CheckProjectileLaunch(dt, commands);
	// Update enemy movement pattern; apply velocity
	UpdateMovementPattern(dt);
	Entity::UpdateCurrent(dt, commands);
}

int	Aircraft::GetIdentifier()
{
	return m_identifier;
}

void Aircraft::SetIdentifier(int identifier)
{
	m_identifier = identifier;
}

void Aircraft::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(m_type)].m_directions;
	if(!directions.empty())
	{
		//Move along the current direction, change direction
		if(m_travelled_distance > directions[m_directions_index].m_distance)
		{
			m_directions_index = (m_directions_index + 1) % directions.size();
			m_travelled_distance = 0.f;
		}

		//Compute velocity from direction
		double radians = Utility::ToRadians(directions[m_directions_index].m_angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		m_travelled_distance += GetMaxSpeed() * dt.asSeconds();

	}


}

float Aircraft::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Aircraft::Fire()
{
	//Only ships with a non-zero fire interval fire
	if(Table[static_cast<int>(m_type)].m_fire_interval != sf::Time::Zero && m_has_ball == true)
	{
		m_is_firing = true;
	}
}

void Aircraft::LaunchMissile()
{
	if(m_missile_ammo > 0)
	{
		m_is_launching_missile = true;
		--m_missile_ammo;
	}
}

void Aircraft::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	//Enemies try and fire as often as possible
	if(!IsAllied())
	{
		Fire();
	}

	//Rate the bullets - default to 2 times a second
	if(m_is_firing && m_fire_countdown <= sf::Time::Zero)
	{
		PlayLocalSound(commands, IsAllied() ? SoundEffect::kAlliedGunfire : SoundEffect::kEnemyGunfire);
		commands.Push(m_fire_command);
		m_fire_countdown += Table[static_cast<int>(m_type)].m_fire_interval / (m_fire_rate + 1.f);
		m_is_firing = false;
	}
	else if(m_fire_countdown > sf::Time::Zero)
	{
		//Wait, can't fire yet
		m_fire_countdown -= dt;
		m_is_firing = false;
		std::cout << "Ball Gone " << std::endl;
		m_has_ball = false;
	}
	//Missile launch
	if(m_is_launching_missile)
	{
		PlayLocalSound(commands, SoundEffect::kLaunchMissile);
		commands.Push(m_missile_command);
		m_is_launching_missile = false;
	}
}

bool Aircraft::IsAllied() const
{
	if (m_TeamPink)
	{
		return m_type == AircraftType::kTeamPink;
	}
	else
		return m_type == AircraftType::kTeamBlue;
}


//TODO Do enemies need a different offset as they are flying down the screen?
void Aircraft::CreateBullets(SceneNode& node, const TextureHolder& textures) const
{


	ProjectileType type = IsAllied() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;

	if (m_has_ball == true) {




		switch (m_spread_level)
		{
		case 1:
			CreateProjectile(node, type, 0.0f, 0.5f, textures);
			break;
		case 2:
			CreateProjectile(node, type, -0.5f, 0.5f, textures);
			CreateProjectile(node, type, 0.5f, 0.5f, textures);
			break;
		case 3:
			CreateProjectile(node, type, -0.5f, 0.5f, textures);
			CreateProjectile(node, type, 0.0f, 0.5f, textures);
			CreateProjectile(node, type, 0.5f, 0.5f, textures);
			break;

		}
		
	}
	//m_has_ball == false;
}

void Aircraft::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset,
	const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);
	//for left side team
	

	
	sf::Vector2f velocity(-projectile->GetMaxSpeed() * 0.8, 0);


	if (type == ProjectileType::kEnemyBullet) {
		sf::Vector2f velocity(projectile->GetMaxSpeed() * 0.8, 0);
	}


	float sign = IsAllied() ? -1.f : +1.f;
	projectile->setPosition(GetWorldPosition() + offset * sign);
	projectile->SetVelocity(velocity * sign);
	projectile->setScale(2.f, 2.f);
	node.AttachChild(std::move(projectile));

	
}

sf::FloatRect Aircraft::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Aircraft::IsMarkedForRemoval() const
{
	return IsDestroyed() && (m_explosion.IsFinished() || !m_show_explosion);
}

void Aircraft::Remove()
{
	Entity::Remove();
	m_show_explosion = false;
}

void Aircraft::CheckPickupDrop(CommandQueue& commands)
{
	if(!IsAllied() && Utility::RandomInt(3) == 0 && !m_spawned_pickup)
	{
		commands.Push(m_drop_pickup_command);
	}
	m_spawned_pickup = true;
}

void Aircraft::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 0.f);
	node.AttachChild(std::move(pickup));
}


void Aircraft::UpdateRollAnimation()
{
	//std::cout << m_current_walk_frame << std::endl;
	if (Table[static_cast<int>(m_type)].m_has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(m_type)].m_walk_texture_rect;
		if (m_is_firing == false && GetVelocity().x < 0.0f || GetVelocity().x > 0.0f || GetVelocity().y < 0.0f || GetVelocity().y > 0.0f) 
		{ 
			textureRect = Table[static_cast<int>(m_type)].m_walk_texture_rect;
			if (m_current_walk_frame == 0 && clock.getElapsedTime().asMilliseconds() > 100.0f)
			{
				time_since_last_frame = clock.getElapsedTime().asSeconds();
				m_current_walk_frame++;
				m_sprite.setTextureRect(textureRect);
			}
			else if (clock.getElapsedTime().asMilliseconds() > time_since_last_frame + 250.0f)
			{
				time_since_last_frame += 250.0f;
				
				textureRect.left += m_current_walk_frame * textureRect.width;
				//std::cout << textureRect.left << std::endl;
				m_current_walk_frame++;
				std::cout << m_current_walk_frame << std::endl;
				//m_sprite.setTextureRect(textureRect);
				if (m_current_walk_frame > Table[static_cast<int>(m_type)].m_walk_animation_frames - 1)
				{
					m_current_walk_frame = 0; 
					clock.restart();
				}
				m_sprite.setTextureRect(textureRect);
			}
		}
		//m_sprite.setTextureRect(textureRect);
	}
}

void Aircraft::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = Category::kSoundEffect;
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
	{
		node.PlaySound(effect, world_position);
	});

	commands.Push(command);
}

void Aircraft::PickUpBall()
{
	m_has_ball = true;
}

bool Aircraft::HasBall()
{
	return m_has_ball;
}




