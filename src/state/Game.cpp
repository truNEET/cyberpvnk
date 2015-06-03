#include <punk/state/Game.hpp>
#include <punk/Engine.hpp>

namespace pk {

namespace state {

Game::Game()
{
	Engine::getSingleton()->removeEventListeners({ sf::Event::KeyPressed, sf::Event::KeyReleased });	
}

void Game::draw(const sf::RenderWindow &rwin)
{
	/* TODO: Draw */
}

void Game::update(const sf::Time &dt)
{
	/* TODO: Update */
}

}

}
