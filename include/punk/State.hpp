#ifndef PK_STATE_HPP
#define PK_STATE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

namespace pk {

class State
{
public:
	virtual void draw(const sf::RenderWindow &rwin) = 0;
	virtual void update(const sf::Time &dt) = 0;
};

}

#endif
