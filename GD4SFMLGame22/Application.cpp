#include "Application.hpp"

#include "GameOverState.hpp"
#include "State.hpp"
#include "StateID.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "MultiplayerGameState.hpp"


const sf::Time Application::kTimePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
:m_window(sf::VideoMode(1024, 768), "Network", sf::Style::Close)
, m_key_binding_1(1)
, m_key_binding_2(2)
, m_stack(State::Context(m_window, m_textures, m_fonts, m_music, m_sounds, m_key_binding_1, m_key_binding_2))
, m_statistics_numframes(0)
{
	m_window.setKeyRepeatEnabled(false);

	m_fonts.Load(Fonts::Main, "Media/Fonts/Sansation.ttf");
	m_textures.Load(Textures::kTitleScreen, "Media/Textures/TitleScreen.png");
	m_textures.Load(Textures::kButtons, "Media/Textures/Buttons.png");

	m_statistics_text.setFont(m_fonts.Get(Fonts::Main));
	m_statistics_text.setPosition(5.f, 5.f);
	m_statistics_text.setCharacterSize(10u);

	RegisterStates();
	m_stack.PushState(StateID::kTitle);
}

void Application::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (m_window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		time_since_last_update += elapsedTime;

		while (time_since_last_update > kTimePerFrame)
		{
			time_since_last_update -= kTimePerFrame;
			ProcessInput();
			Update(kTimePerFrame);

			if(m_stack.IsEmpty())
			{
				m_window.close();
			}
		}
		UpdateStatistics(elapsedTime);
		Render();
	}
}

void Application::ProcessInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_stack.HandleEvent(event);
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
	}
}

void Application::Update(sf::Time delta_time)
{
	m_stack.Update(delta_time);
}

void Application::Render()
{
	m_window.clear();
	m_stack.Draw();

	m_window.setView(m_window.getDefaultView());
	m_window.draw(m_statistics_text);
	m_window.display();
}

void Application::UpdateStatistics(sf::Time elapsed_time)
{
	m_statistics_updatetime += elapsed_time;
	m_statistics_numframes += 1;

	if (m_statistics_updatetime >= sf::seconds(1.0f))
	{
		m_statistics_text.setString(
			"Frames / Second = " + std::to_string(m_statistics_numframes) + "\n" +
			"Time / Update = " + std::to_string(m_statistics_updatetime.asMicroseconds() / m_statistics_numframes) + "us");

		m_statistics_updatetime -= sf::seconds(1.0f);
		m_statistics_numframes = 0;
	}
}

void Application::RegisterStates()
{
	m_stack.RegisterState<TitleState>(StateID::kTitle);
	m_stack.RegisterState<MenuState>(StateID::kMenu);
	m_stack.RegisterState<GameState>(StateID::kGame);
	m_stack.RegisterState<MultiplayerGameState>(StateID::kHostGame, true);
	m_stack.RegisterState<MultiplayerGameState>(StateID::kJoinGame, false);
	m_stack.RegisterState<PauseState>(StateID::kPause);
	m_stack.RegisterState<PauseState>(StateID::kNetworkPause, true);
	m_stack.RegisterState<SettingsState>(StateID::kSettings);
	m_stack.RegisterState<GameOverState>(StateID::kGameOver, "Mission Failed!");
	m_stack.RegisterState<GameOverState>(StateID::kMissionSuccess, "Mission Successful!");
}
