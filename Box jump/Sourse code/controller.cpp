/////////////////////////////////////////////////
// Include
#include "controller.h"
#include "tools\pool.h"
#include "game.h"
#include "tools\utils.h"
#include "gameObject\platform.h"
#include "gameObject\cloud.h"
#include "dataManager\dataManager.h"

/////////////////////////////////////////////////
// Variables
int maxYplatformIndex = 0;
int maxYcloudIndex = 2;

/////////////////////////////////////////////////
// Class Controller
Controller::Controller()
{
	m_game = 0;
	m_platformPool = 0;
	m_cloudPool = 0;
}

void Controller::initialize(Game* game, Pool* platformPool, Pool* cloudPool, Camera* camera)
{
	for (int i = 0; i < kMaxObjectsCount; i++)
	{
		m_objectsList[i] = 0;
	}

	maxYplatformIndex = 0;
	maxYcloudIndex = 2;

	m_platformPool = platformPool;
	m_cloudPool = cloudPool;
	m_game = game;
	m_camera = camera;
	loadLevel();
}

void Controller::loadLevel()
{
	// Create platforms under the players
	if(m_game->getGameMode() == GameMode_OnePlayer)
	{
		m_objectsList[0] = m_platformPool->getNewObject();
		m_objectsList[0]->setPosition(Vec2(0, 175));
		m_objectsList[0]->setDepth(1);
		m_objectsList[0]->setActive(true);
	}

	if (m_game->getGameMode() == GameMode_TwoPlayers)
	{
		m_objectsList[0] = m_platformPool->getNewObject();
		m_objectsList[0]->setPosition(Vec2(-100, 175));
		m_objectsList[0]->setDepth(1);
		m_objectsList[0]->setActive(true);

		m_objectsList[1] = m_platformPool->getNewObject();
		m_objectsList[1]->setPosition(Vec2(100, 175));
		m_objectsList[1]->setDepth(1);
		m_objectsList[1]->setActive(true);
	}

	m_objectsList[2] = m_cloudPool->getNewObject();
	m_objectsList[2]->setPosition(Vec2(-200, 100));
	m_objectsList[2]->setDepth(2);
	m_objectsList[2]->setActive(true);
}

void Controller::update(float dt)
{
	for(int i = 0; i < kMaxObjectsCount; i++)
	{
		if (m_objectsList[i] != 0)
		{
			if (m_objectsList[i]->getType() == GameObjectType_Platform)
			{
				Platform* platform = (Platform*)m_objectsList[i];
				if (m_camera->getPosition().y - platform->getPosition().y < -(kScreenHeight * 1.5) || !platform->getActive())
				{
					m_platformPool->deleteObject(m_objectsList[i]);
					m_objectsList[i] = 0;
				}
				else if (platform->getStartPosition().y < ((Platform*)m_objectsList[maxYplatformIndex])->getStartPosition().y)
				{
					maxYplatformIndex = i;
				}
			}
			else if (m_objectsList[i]->getType() == GameObjectType_Cloud)
			{
				Cloud* cloud = (Cloud*)m_objectsList[i];

				if (cloud->getPosition().x - m_camera->getPosition().x > (m_camera->getSize().x / 1.8f))
					cloud->addToPosition(Vec2(-m_camera->getSize().x - GetRandomFloat(30, 80), GetRandomInt(-30, 30)));

				if (cloud->getPosition().y - m_camera->getPosition().y > kScreenHeight || !cloud->getActive())
				{
					m_platformPool->deleteObject(m_objectsList[i]);
					m_objectsList[i] = 0;
				}
				else if (cloud->getPosition().y < m_objectsList[maxYcloudIndex]->getPosition().y)
				{
					maxYcloudIndex = i;
				}
			}
		}
	}

	if (m_camera->getCenterPosition().y - m_objectsList[maxYcloudIndex]->getPosition().y < kScreenHeight /2)
	{
		createNextCloud(m_objectsList[maxYcloudIndex]->getPosition());
	}

	if (abs(m_objectsList[maxYplatformIndex]->getPosition().y - m_camera->getPosition().y) < kScreenHeight * 2)
	{
		createNextPlatforms(m_objectsList[maxYplatformIndex]->getPosition());
	}
}

void Controller::createNextCloud(Vec2 cloudPos)
{
	int i = 0;
	while (m_objectsList[i] != 0) i++;

	cloudPos = Vec2(-GetRandomFloat(30, 100), cloudPos.y - GetRandomInt(50, 175));

	m_objectsList[i] = m_cloudPool->getNewObject();
	m_objectsList[i]->setPosition(cloudPos);

	sf::IntRect rect = DataManager::instance()->getSpriteContainer("CloudSprite")->rect;
	rect.top += GetRandomInt(0, 2) * 16;
	m_objectsList[i]->setTextureRect(rect);

	if (m_objectsList[i]->getDepth() != 2) m_objectsList[i]->setDepth(2);
	m_objectsList[i]->setActive(true);
	maxYcloudIndex = i; 

	if (m_camera->getCenterPosition().y - m_objectsList[maxYcloudIndex]->getPosition().y < kScreenHeight / 2)
	{
		createNextCloud(m_objectsList[maxYcloudIndex]->getPosition());
	}
}

void Controller::createNextPlatforms(Vec2 currentPlatformPos)
{
	Vec2 offset;

	int p = 0, t = GetRandomInt(0, 10);
	Vec2 maxPosition = Vec2(0, 250);

	/*if (t > 9) p = 3;
	else */if(t > 6) p = 2;
	else p = 1;

	float magnitude = 90;
	
	for (int j = 0; j < p; j++)
	{
		offset.y = -GetRandomInt(magnitude / 2.5f, magnitude);
		offset.x = GetRandomFloat( -magnitude + 2*magnitude/p * j, - magnitude + 2*magnitude/p * (j+1) );

		offset = offset.magnitude(magnitude);

		Vec2 newPosition = currentPlatformPos + offset;

		if (newPosition.x < -kScreenWidth / 2 - 16) newPosition.x = (kScreenWidth / 2 + 16) + newPosition.x - (-kScreenWidth / 2 - 16);
		if (newPosition.x > kScreenWidth / 2 - 16) newPosition.x = (-kScreenWidth / 2 - 16) + newPosition.x - (kScreenWidth / 2 + 16);

		int i = 0;
		while (m_objectsList[i] != 0) i++;

		m_objectsList[i] = (Platform*) m_platformPool->getNewObject();
		m_objectsList[i]->setPosition(newPosition);
		if (m_objectsList[i]->getDepth() != 1) m_objectsList[i]->setDepth(1);
		((Platform*)m_objectsList[i])->initialize();

		if (newPosition.y < maxPosition.y) maxPosition = newPosition;
	}
	if (abs(maxPosition.y - m_camera->getPosition().y) < kScreenHeight * 2) createNextPlatforms(maxPosition);
}