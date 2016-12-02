#pragma once
#include <SFML/Graphics.hpp>

class Interactive
{
public:
	virtual void HandleKeyboard(sf::Keyboard::Key key) = 0;
	virtual void HandleMouse(sf::Mouse::Button mb, sf::Vector2f& mousePosition) = 0;

};