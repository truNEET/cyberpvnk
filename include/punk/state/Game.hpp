#ifndef PK_STATE_GAME_HPP
#define PK_STATE_GAME_HPP

#include <punk/State.hpp>

namespace pk {

namespace state {

class Game : public State
{
public:
	void draw(const sf::RenderWindow &rwin);
	void update(const sf::Time &dt);
};

}

}

#endif
