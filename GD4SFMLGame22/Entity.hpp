#pragma once
#include "CommandQueue.hpp"
#include "SceneNode.hpp"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);
	sf::Vector2f GetVelocity() const;

	int GetHitPoints() const;
	void SetHitpoints(int points);
	void Repair(unsigned int points);
	void Damage(int points);
	virtual void Remove();
	void Destroy();
	virtual bool IsDestroyed() const override;

protected:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);

private:
	sf::Vector2f m_velocity;
	int m_hitpoints;
};