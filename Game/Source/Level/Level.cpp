#include <Level/Level.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <Entity/Impl/BaseEntity.hpp>
#include <Entity/Impl/TeleportEntity.hpp>


Level::Level()
:
m_quadTree(nullptr)
{

}

Level::~Level()
{
	delete m_quadTree;
}

void Level::Initialize(const std::string& filePath)
{
	m_quadTree = new Node();
	LoadFile(filePath);
}

void Level::LoadFile(const std::string& filePath)
{
	using namespace std;
	ifstream file(filePath, std::ios::binary);
	if (file.is_open())
	{
		string line;
		size_t width, height, x, y, maxColor;
		width = height = x = y = maxColor= 0;
		getline(file, line); // Max color size
		file >> width >> height; // width, height
		m_quadTree->Initialize(sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(width * TILE_SIZE, height * TILE_SIZE)));
		getline(file, line); // Max color size
		file >> maxColor;

		size_t r, g, b;
		r = g = b = 0;
		while (file >> r >> g >> b)
		{
			m_quadTree->AddEntity(EvaluateTileDataFromFile(r, g, b, x, y));
			++x;
			if (x == width-1)
			{
				x = 0;
				++y;
			}
		}
		file.close();
	}
	else
	{
		cout << "Error opening levelfile: " << filePath << endl;
	}
}

BaseEntity* Level::EvaluateTileDataFromFile(size_t r, size_t g, size_t b, size_t x, size_t y)
{
	BaseEntity* returnEntity = nullptr;
	if(EmptyCondition(r,g,b))
	{
		returnEntity = new BaseEntity(EntityType::EMPTY);
		returnEntity->Initialize(TILE_SIZE * x, TILE_SIZE * y, GetTextureFromColorCoding(g));
	}
	else if (SimleWallCondition(r, g, b))
	{
		returnEntity = new BaseEntity(EntityType::WALL);
		returnEntity->Initialize(TILE_SIZE * x, TILE_SIZE * y, GetTextureFromColorCoding(g));
	}
	else if (PlayerSpawnCondition(r, g, b))
	{
//		m_spawnPositions.push_back(sf::Vector2i(x, y));
//		returnEntity = new TeleportEntity();
	}
	else if (TeleporterCondition(r, g, b))
	{
		m_telePorterPositions.push_back(sf::Vector2i(x, y));
	}
	if (returnEntity != nullptr)
		m_all.push_back(returnEntity);
	return returnEntity;
}

bool Level::PlayerSpawnCondition(size_t r, size_t g, size_t b)
{
	if (r == 255 && g == 0 && b == 255)
	{
		return true;
	}
	return false;
}

bool Level::SimleWallCondition(size_t r, size_t g, size_t b)
{
	if (r == 1)
	{
		return true;
	}
	return false;
}

bool Level::TeleporterCondition(size_t r, size_t g, size_t b)
{
	if (r == 0 && g == 0 && b == 255)
	{
		return true;
	}
	return false;
}

bool Level::EmptyCondition(size_t r, size_t g, size_t b)
{
	if (r == 0 && g == 0 && b == 0)
	{
		return true;
	}
	return false;
}


void Level::Render(sf::RenderWindow* window)
{
	for (auto i : m_all)
	{
		i->Render(window);
	}
}

std::vector<BaseEntity*>* Level::FindNearObjects(sf::FloatRect rectangle, std::vector<BaseEntity*>* destination)
{
	return m_quadTree->FindNearObjects(rectangle, destination);
}

std::string Level::GetTextureFromColorCoding(size_t g)
{
	std::string texture;

	switch (g)
	{
	case 0:
	//	texture = "character_new.png";
		break;
	case 1:
		texture = "dirt_tile.jpg";
		break;
	case 2:
		texture = "ground_tile_grass.jpg";
		break;
	case 3:
		texture = "stone_tile.png";
		break;
	case 4:
		texture = "";
		break;
	case 5:
		texture = "";
		break;
	default:
		break;
	}
	return texture;
}