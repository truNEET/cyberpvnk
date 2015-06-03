#ifndef PK_CONTROLLABLE_HPP
#define PK_CONTROLLABLE_HPP

#include <punk/InputController.hpp>

namespace pk {

class Controllable
{
public:
	Controllable();
protected:
	std::shared_ptr<InputController> m_input;
};

}
