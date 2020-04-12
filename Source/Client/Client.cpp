#include "SFML/Graphics.hpp"
#include <iostream>

int main() 
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Untitled Game");

	while (window.isOpen()) 
	{
		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if (e.type == sf::Event::Closed) 
			{
				window.close();
			}
		}

		window.display();
	}

	return 0;
}