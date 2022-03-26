#include "SceneNode.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Utility.hpp"

SceneNode::SceneNode(Category::Type category):m_children(), m_parent(nullptr), m_default_category(category)
{
}

void SceneNode::AttachChild(Ptr child)
{
	child->m_parent = this;
	//Todo - Why is emplace_back more efficient than push_back
	m_children.emplace_back(std::move(child));
}

SceneNode::Ptr SceneNode::DetachChild(const SceneNode& node)
{
	auto found = std::find_if(m_children.begin(), m_children.end(), [&](Ptr& p) {return p.get() == &node; });
	assert(found != m_children.end());

	Ptr result = std::move(*found);
	result->m_parent = nullptr;
	m_children.erase(found);
	return result;
}

void SceneNode::Update(sf::Time dt, CommandQueue& commands)
{
	UpdateCurrent(dt, commands);
	UpdateChildren(dt, commands);
}

sf::Vector2f SceneNode::GetWorldPosition() const
{
	return GetWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::GetWorldTransform() const
{
	sf::Transform transform = sf::Transform::Identity;
	for(const SceneNode* node = this; node != nullptr; node = node->m_parent)
	{
		transform = node->getTransform() * transform;
	}
	return transform;
}

void SceneNode::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	//Do nothing by default
}

void SceneNode::UpdateChildren(sf::Time dt, CommandQueue& commands)
{
	for(Ptr& child : m_children)
	{
		child->Update(dt, commands);
	}
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Apply transform of the current node
	states.transform *= getTransform();

	//Draw the node and children with changed transform
	DrawCurrent(target, states);
	DrawChildren(target, states);
	sf::FloatRect rect = GetBoundingRect();
	//DrawBoundingRect(target, states, rect);
}

void SceneNode::DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const
{
	//Do nothing by default
}

void SceneNode::DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const Ptr& child : m_children)
	{
		child->draw(target, states);
	}
}

void SceneNode::OnCommand(const Command& command, sf::Time dt)
{
	//Is this command for me?
	if(command.category & GetCategory())
	{
		command.action(*this, dt);
	}

	//Pass command on to children
	for(Ptr& child : m_children)
	{
		child->OnCommand(command, dt);
	}
}

unsigned int SceneNode::GetCategory() const
{
	return static_cast<int>(m_default_category);
}

float Distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return Utility::Length(lhs.GetWorldPosition() - rhs.GetWorldPosition());
}

sf::FloatRect SceneNode::GetBoundingRect() const
{
	return sf::FloatRect();
}

void SceneNode::DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, sf::FloatRect& rect) const
{
	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(rect.left, rect.top));
	shape.setSize(sf::Vector2f(rect.width, rect.height));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1.f);
	target.draw(shape);
}

bool Collision(const SceneNode& lhs, const SceneNode& rhs)
{
	return lhs.GetBoundingRect().intersects(rhs.GetBoundingRect());
}

void SceneNode::CheckNodeCollision(SceneNode& node, std::set<Pair>& collision_pairs)
{
	if(this != &node && Collision(*this, node) && !IsDestroyed() && !node.IsDestroyed())
	{
		collision_pairs.insert(std::minmax(this, &node));
	}
	for(Ptr& child : m_children)
	{
		child->CheckNodeCollision(node, collision_pairs);
	}
}

void SceneNode::CheckSceneCollision(SceneNode& scene_graph, std::set<Pair>& collision_pairs)
{
	CheckNodeCollision(scene_graph, collision_pairs);
	for(Ptr& child : scene_graph.m_children)
	{
		CheckSceneCollision(*child, collision_pairs);
	}
}

bool SceneNode::IsDestroyed() const
{
	//What should the default for a Scenenode be
	return false;
}

bool SceneNode::IsMarkedForRemoval() const
{
	return IsDestroyed();
}

void SceneNode::RemoveWrecks()
{
	auto wreck_field_begin = std::remove_if(m_children.begin(), m_children.end(), std::mem_fn(&SceneNode::IsMarkedForRemoval));
	m_children.erase(wreck_field_begin, m_children.end());
	std::for_each(m_children.begin(), m_children.end(), std::mem_fn(&SceneNode::RemoveWrecks));
}
