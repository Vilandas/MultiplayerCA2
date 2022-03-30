#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"
#include "PickupType.hpp"
#include "ResourceIdentifiers.hpp"

class Aircraft;

class Pickup : public Entity
{
public:
	Pickup(PickupType type, const TextureHolder& textures);
	Pickup(PickupType type, const TextureHolder& textures,int index);
	virtual unsigned int GetCategory() const override;
	virtual sf::FloatRect GetBoundingRect() const;
	void Apply(Aircraft& player) const;
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	int GetIndex();

private:
	PickupType m_type;
	sf::Sprite m_sprite;
	int m_index;
};

