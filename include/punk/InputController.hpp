#ifndef PK_INPUTCONTROLLER_HPP
#define PK_INPUTCONTROLLER_HPP

#include <punk/Engine.hpp>

namespace pk {

class InputController
{
public:
	InputController();
protected:
	std::shared_ptr<Engine> m_engine;	
};

}

#endif
