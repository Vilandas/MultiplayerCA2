#include "SoundNode.hpp"

#include "SoundNode.hpp"
#include "SoundPlayer.hpp"


SoundNode::SoundNode(SoundPlayer& player)
	: m_sounds(player)
{
}

void SoundNode::PlaySound(SoundEffect sound, sf::Vector2f position)
{
	m_sounds.Play(sound, position);
}

unsigned int SoundNode::GetCategory() const
{
	return Category::kSoundEffect;
}