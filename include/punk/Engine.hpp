#ifndef PK_ENGINE_HPP
#define PK_ENGINE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <punk/State.hpp>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <map>

namespace pk {

class Engine
{
public:
	Engine();
	static std::shared_ptr<Engine> getSingleton();
	int run(int argc, char **argv);
	void addEventListener(const sf::Event::EventType &type, std::function<void(sf::Event&)> cb);
private:
	static std::shared_ptr<Engine> m_instance;
	std::map<sf::Event::EventType, std::vector<std::function<void(sf::Event&)>>> m_events;
	std::stack<std::shared_ptr<State>> m_states;
	sf::RenderWindow m_rwin;
};

}

#endif
