#pragma once
#include "Fonts.hpp"
#include "Shaders.hpp"
#include "SoundEffect.hpp"
#include "Textures.hpp"

//Forward declaration of SFML texture class
namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

template<typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts> FontHolder;
typedef ResourceHolder<sf::Shader, ShaderTypes> ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect> SoundBufferHolder;