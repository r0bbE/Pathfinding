#include "Map.h"

Map::Map(float screenWidth, float screenHeight, const std::string& filename)
{
	LoadMapFromImage(screenWidth, screenHeight, filename);
}

Map::~Map()
{
	for(auto line : shapes)
	{
		for (auto hex : line)
		{
			delete hex;
		}
	}
}

void Map::Render(sf::RenderWindow* window)
{
	
	for(const auto line : shapes)
	{
		for(const auto hexdat : line)
		{
			window->draw(*(hexdat->hex));
		}
	}

	if (selectedHexDat != nullptr)
	{
		Hexagon drawSelected = *selectedHexDat->hex;
		drawSelected.setOutlineColor(sf::Color::Red);
		window->draw(drawSelected);
	}
}

void Map::DebugRender(sf::RenderWindow* window)
{

	if (selectedHexDat != nullptr)
	{
		sf::Vector2i evenOffsets[6] = {sf::Vector2i(0,1), sf::Vector2i(1,0), sf::Vector2i(0,-1), sf::Vector2i(-1,0), sf::Vector2i(+1,-1), sf::Vector2i(+1, +1)};
		sf::Vector2i oddOffsets[6] = {sf::Vector2i(0,1), sf::Vector2i(1,0), sf::Vector2i(0,-1), sf::Vector2i(-1,0), sf::Vector2i(-1,+1), sf::Vector2i(-1, -1)};

		Hexagon drawSelected;
		sf::Vector2i* offsets = selectedHexDat->index.y % 2 == 0 ? evenOffsets : oddOffsets;

		for(auto i = 0; i < 6; ++i)
		{
			int xindex = selectedHexDat->index.x + offsets[i].x;
			int yindex = selectedHexDat->index.y + offsets[i].y;

			if(xindex >= 0 && xindex < rows && yindex >= 0 && yindex < columns)
			{
				drawSelected = *shapes[xindex][yindex]->hex;
				drawSelected.setOutlineColor(sf::Color::Cyan);
				window->draw(drawSelected);
			}
		}

		drawSelected = *selectedHexDat->hex;
		drawSelected.setOutlineColor(sf::Color::Red);
		window->draw(drawSelected);
		
	}

	DebugRenderText(window);
}

void Map::DebugRenderText(sf::RenderWindow* window)
{
	//DebugText - Render Indices on Hexes
	sf::Text text;
	sf::Font t;
	t.loadFromFile("./Assets/arial.ttf");
	text.setFont(t);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::White);
	text.setOutlineThickness(2);
	text.setOutlineColor(sf::Color::Black);

	for (const auto line : shapes)
	{
		for (const auto hexdat : line)
		{
			text.setPosition(hexdat->hex->getPosition());
			text.setString(std::to_string(hexdat->index.x) + "," + std::to_string(hexdat->index.y));
			text.setOrigin(text.getGlobalBounds().width / 2.0f, text.getGlobalBounds().height / 2.0f);
			window->draw(text);
		}
	}
}

void Map::HandleKeyboard(sf::Keyboard::Key key)
{

}

void Map::HandleMouse(sf::Mouse::Button mb, sf::Vector2f& mousePosition)
{
	sf::Vector2f mousePos = sf::Vector2f(mousePosition);

	SetCurrentHex(mousePos);

}

float Map::GetHexSize() const
{
	return hexsize * 2.0f;
}

sf::Vector2f Map::GetPositionByIndex(int x, int y)
{
	return shapes[x][y]->hex->getPosition();
}

sf::Vector2f Map::GetPositionByIndex(sf::Vector2i posIndex)
{
	return shapes[posIndex.x][posIndex.y]->hex->getPosition();
}

float Map::distanceBetweenFloatPoints(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

void Map::SetCurrentHex(const sf::Vector2f& mousePos)
{
	float closestDist = std::numeric_limits<float>::max();
	float dist;

	for (const auto line : shapes)
	{
		for (const auto hexdat : line)
		{
			dist = distanceBetweenFloatPoints(mousePos, hexdat->hex->getPosition());
			if (dist < closestDist)
			{
				closestDist = dist;
				selectedHexDat = hexdat;
			}
		}
	}
}

void Map::LoadMapFromImage(float screenWidth, float screenHeight, const std::string& filename)
{
	sf::Image mapImage;
	mapImage.loadFromFile(filename);

	GenerateFromImage(screenWidth, screenHeight, mapImage);
}

void Map::GenerateFromImage(float screenWidth, float screenHeight, const sf::Image& mapImage)
{
	int imageWidth = mapImage.getSize().x;
	int imageHeight = mapImage.getSize().y;

	columns = imageWidth;
	rows = imageHeight;
	float outlineThickness = 2.0f;

	float hexWidth = (screenWidth / columns) * 3.0f/4.0f * 0.83f;
	float hexHeight = ((screenHeight) / rows) * 0.53f;

	hexsize = hexWidth < hexHeight ? hexWidth : hexHeight;

	Hexagon sizes(hexsize);

	float initialOffsetX = (sizes.width() + outlineThickness) * (3.0f / 4.0f);
	float initialOffsetY = (sizes.height() + outlineThickness) * 1.3f;
	float offsetX = initialOffsetX;
	float offsetY = initialOffsetY;

	shapes.resize(rows);

	for (int i = 0; i < rows; ++i)
	{
		shapes[i].resize(columns);

		for (int j = 0; j < columns; ++j)
		{
			HexData *hexDatTmp = new HexData();

			Hexagon* tmp = new Hexagon(sizes);
			tmp->setOutlineColor(sf::Color::Blue);
			tmp->setOutlineThickness(outlineThickness);
			tmp->setPosition(offsetX, offsetY);

			sf::Color currentColor = mapImage.getPixel(j, i);

			if(currentColor == water)
			{
				 hexDatTmp->terrain = waterDifficulty;
				tmp->setFillColor(water);
			} else if(currentColor == sand)
			{
				hexDatTmp->terrain = sandDifficulty;
				tmp->setFillColor(sand);
			} else if(currentColor == mud)
			{
				hexDatTmp->terrain = mudDifficulty;
				tmp->setFillColor(mud);
			} else if(currentColor == grass)
			{
				hexDatTmp->terrain = grassDifficulty;
				tmp->setFillColor(grass);
			} else if(currentColor == street)
			{
				hexDatTmp->terrain = streetDifficulty;
				tmp->setFillColor(street);
			} else
			{
				hexDatTmp->terrain = notfoundDifficulty;
				tmp->setFillColor(notfoundColor);
			}

			hexDatTmp->hex = tmp;
			hexDatTmp->index.x = i;
			hexDatTmp->index.y = j;
			shapes[i][j] = hexDatTmp;

			offsetX += sizes.width() * 3.0f / 4.0f + outlineThickness / 2.0f;

			if (j % 2 == 0)
			{
				offsetY -= sizes.height() / 2.0f + outlineThickness / 2.0f;
			}
			else
			{
				offsetY += sizes.height() / 2.0f + outlineThickness / 2.0f;
			}
		}

		offsetX = initialOffsetX;
		offsetY = initialOffsetY + (i + 1) * (sizes.height() + outlineThickness);
	}

}
