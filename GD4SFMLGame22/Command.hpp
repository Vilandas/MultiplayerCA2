#pragma once
#include "Category.hpp"
#include <SFML/System/Time.hpp>
#include <functional>
#include <cassert>

class SceneNode;

struct Command
{
	Command();
	std::function<void(SceneNode&, sf::Time)> action;
	unsigned int category;
};

template<typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> DerivedAction(Function fn)
{
	return [=](SceneNode& node, sf::Time dt)
	{
		//Check if cast is safe
		assert(dynamic_cast<GameObject*>(&node) != nullptr);

		//Downcast node and invoke the function
		fn(static_cast<GameObject&>(node), dt);
	};
}



