#include <punk/Engine.hpp>

int main(int argc, char **argv)
{
	return pk::Engine::getSingleton()->run(argc, argv);
}
