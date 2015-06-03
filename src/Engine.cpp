#include <punk/state/Game.hpp>
#include <punk/Engine.hpp>

namespace pk {

std::shared_ptr<Engine> Engine::m_instance = nullptr;

Engine::Engine()
{
	/* TODO: Constructor */
}

std::shared_ptr<Engine> Engine::getSingleton()
{
	if (m_instance == nullptr) {
		m_instance = std::make_shared<Engine>();
	}
	return m_instance;
}

void Engine::addEventListener(const sf::Event::EventType &type, std::function<void(sf::Event&)> cb)
{
	m_events[type].push_back(cb);
}

void Engine::removeEventListeners(const sf::Event::EventType &type)
{
	if (m_events.find(type) != m_events.end()) {
		m_events[type].clear();
	}
}

void Engine::removeEventListeners(const std::vector<sf::Event::EventType> &events)
{
	for (auto &event : events) {
		removeEventListeners(event);
	}
}

int Engine::run(int argc, char **argv)
{
	m_rwin.create(sf::VideoMode(1067, 600, 32), "cyberpvnk", !sf::Style::Resize | sf::Style::Close);

	m_states.push(std::make_shared<state::Game>());

	addEventListener(sf::Event::Closed, [&] (const sf::Event &event) -> void {
		m_rwin.close();
	});

	while (m_rwin.isOpen()) {
		sf::Event event;
		while (m_rwin.pollEvent(event)) {
			if (m_events.find(event.type) != m_events.end()) {
				for (auto &cb : m_events[event.type]) {
					cb(event);
				}
			}
		}
		m_rwin.clear();
		m_states.top()->draw(m_rwin);
		m_states.top()->update(sf::seconds(1));
		m_rwin.display();
	}

	return 0;
}

}
