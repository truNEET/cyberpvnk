#ifndef PK_INPUT_PLAYER_HPP
#define PK_INPUT_PLAYER_HPP

#include <punk/InputController.hpp>

namespace pk {

namespace input {

class Player : public InputController
{
public:
	Player();
protected:
	bool getMovingRight();
	bool getMovingLeft();
private:
	bool m_right;
	bool m_left;
};

}

}

#endif
