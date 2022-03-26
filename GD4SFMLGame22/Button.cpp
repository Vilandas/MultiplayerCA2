#include "Button.hpp"
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "ButtonType.hpp"

namespace GUI
{
	Button::Button(State::Context context)
	: m_sprite(context.textures->Get(Textures::kButtons))
	, m_text("", context.fonts->Get(Fonts::Main), 16)
	, m_is_toggle(false)
	, m_sounds(*context.sounds)
	{
		ChangeTexture(ButtonType::Normal);
		sf::FloatRect bounds = m_sprite.getLocalBounds();
		m_text.setPosition(bounds.width / 2, bounds.height / 2);

	}

	void Button::SetCallback(Callback callback)
	{
		m_callback = std::move(callback);
	}

	void Button::SetText(const std::string& text)
	{
		m_text.setString(text);
		Utility::CentreOrigin(m_text);
	}

	void Button::SetToggle(bool flag)
	{
		m_is_toggle = flag;
	}

	bool Button::IsSelectable() const
	{
		return true;
	}

	void Button::Select()
	{
		Component::Select();
		ChangeTexture(ButtonType::Selected);
	}

	void Button::Deselect()
	{
		Component::Deselect();
		ChangeTexture(ButtonType::Normal);
	}

	void Button::Activate()
	{
		Component::Activate();
		//If toggle then show button is pressed or toggled
		if(m_is_toggle)
		{
			ChangeTexture(ButtonType::Pressed);
		}
		if(m_callback)
		{
			m_callback();
		}
		if(!m_is_toggle)
		{
			Deactivate();
		}
		m_sounds.Play(SoundEffect::kButton);
	}

	void Button::Deactivate()
	{
		Component::Deactivate();
		if(m_is_toggle)
		{
			if(IsSelected())
			{
				ChangeTexture(ButtonType::Selected);
			}
			else
			{
				ChangeTexture(ButtonType::Normal);
			}
		}
	}

	void Button::HandleEvent(const sf::Event& event)
	{
	}

	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_sprite, states);
		target.draw(m_text, states);
	}

	void Button::ChangeTexture(ButtonType buttonType)
	{
		sf::IntRect textureRect(0, 50 * static_cast<int>(buttonType), 200, 50);
		m_sprite.setTextureRect(textureRect);
	}
}
