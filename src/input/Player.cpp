#include <punk/input/Player.hpp>

namespace pk {

namespace input {

Player::Player() :
	InputController()
{
	m_right = false;
	m_left = false;

	m_engine->addEventListener(sf::Event::KeyPressed, [&] (const sf::Event &event) -> void {
		switch (event.type) {
			case sf::Keyboard::Right:	m_right = true; break;
			case sf::Keyboard::Left:	m_left = true; break;
		}
	});

	m_engine->addEventListener(sf::Event::KeyReleased, [&] (const sf::Event &event) -> void {
		switch (event.type) {
			case sf::Keyboard::Right:	m_right = false;
			case sf::Keyboard::Left:	m_left = false; break;
		}
	});
}

bool Player::getMovingRight()
{
	return m_right && !m_left;
}

bool Player::getMovingLeft()
{
	return !m_right && m_left;
}

}

}
