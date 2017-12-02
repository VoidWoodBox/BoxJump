/////////////////////////////////////////////////
// Include
#include "hero.h"
#include "utils.h"
#include "levelSettings.h"
#include "settingsManager.h"
#include "platform.h"

#include <iostream>
#include <Windows.h>

/////////////////////////////////////////////////
// Global 
Vec2 enterDirection = Vec2();

extern SettingsManager settingsManager;

float maxSpeed = 420;
float minSpeed = -450;
int t = 0;

/////////////////////////////////////////////////
// Class Hero

Hero::Hero()
{
	m_type = GameObjectType_Hero;
	m_heroSkin = HeroSkin_Chest;

	m_leftKey = 0;
	m_rightKey = 0;
	m_jumpKey = 0;

	m_heroAverageSpeed = 0;

	m_material.density = settingsManager.p_objectsSettings->get<float>("Wood.Density");
	m_material.restitution = settingsManager.p_objectsSettings->get<float>("Wood.Restitution");
	m_material.staticFriction = settingsManager.p_objectsSettings->get<float>("Wood.StaticFriction");
	m_material.dynamicFriction = settingsManager.p_objectsSettings->get<float>("Wood.DynamicFriction");

	m_heroJumpForse = settingsManager.p_objectsSettings->get<float>("Hero.JumpForse") *0.5;
	m_heroMoveForse = settingsManager.p_objectsSettings->get<float>("Hero.MoveForse");
	m_heroMaxSpeed = settingsManager.p_objectsSettings->get<float>("Hero.MaxSpeed");

	m_jump_key_pressed_in_last_frame = false;
}

void Hero::update(float dt)
{
	// Hero move
	if (IsKeyDown(m_leftKey))
	{
		m_heroSpeed = -m_heroMoveForse;
	}

	if (IsKeyDown(m_rightKey))
	{
		m_heroSpeed = m_heroMoveForse;
	}

	if (IsKeyDown(m_rightKey) == IsKeyDown(m_leftKey))
		m_heroSpeed = 0;


	if (!IsKeyDown(m_jumpKey)) m_jump_key_pressed_in_last_frame = false;

	GameObject::update(dt);
}

void Hero::spriteUpdate()
{

	if (m_velocity.y < minSpeed / 4 * 3) m_sprite->setTextureRect(sf::IntRect(16, 21 + 24 * m_heroSkin, 16, 19));
	else if (m_velocity.y < 20) m_sprite->setTextureRect(sf::IntRect(0, 21 + 24 * m_heroSkin, 16, 19));
	else if (m_velocity.y < maxSpeed / 10) m_sprite->setTextureRect(sf::IntRect(32, 21 + 24 * m_heroSkin, 16, 19));
	else if (m_velocity.y < maxSpeed / 5) m_sprite->setTextureRect(sf::IntRect(48, 21 + 24 * m_heroSkin, 16, 19));
	else m_sprite->setTextureRect(sf::IntRect(64, 21 + 24 * m_heroSkin, 16, 19));

	if ( m_velocity.y - m_heroAverageSpeed < -35 && m_heroAverageSpeed > 0) m_sprite->setTextureRect(sf::IntRect(80, 21 + 24 * m_heroSkin, 16, 19));

	/*
	t++;
	if (t > 25)
	{
		t -= 25;
		std::cout << maxSpeed << "\t" << minSpeed << "\n";
	}
	// */

	m_heroAverageSpeed = (m_heroAverageSpeed * 9/10 + m_velocity.y / 10);
}

void Hero::physicsUpdate(float dt)
{
	if ((m_position - m_camera->getPosition()).y < 175) m_game->setRequiredCameraPos(m_position.y + 175);

	if (IsKeyDown(m_jumpKey))
		m_jump_key_pressed_in_last_frame = true;

	if (fabs(m_velocity.x) < m_heroMaxSpeed || m_heroSpeed * m_velocity.x < 0) 
		addForse( Vec2(m_heroSpeed, 0) );

	// Add air resistance
	addForse(m_velocity * - 1);

	spriteUpdate();

	GameObject::physicsUpdate(dt);
}

void Hero::intersect(Collision* collision)
{
	if (collision->collisionWith(GameObjectType_Platform, GameObjectType_Hero)
		&& 
		(collision->getNormal().y >= 0 || ( collision->getState() == CollisionState_Stay && collision->getEnterDirection().y >= 0) )
	)
	{
		collision->m_needToResolve = false;
		return;
	}

	
	if(	(collision->collisionWith(GameObjectType_Platform, GameObjectType_Hero) || collision->collisionWith(GameObjectType_Hero, GameObjectType_Hero))
		&& 
		collision->getNormal().y < 0 
	)
	{
		GameObject* tmp = 0;
		if (collision->getObjects().object1 == ((GameObject*)this)) 
			tmp = collision->getObjects().object2;
		else 
			tmp = collision->getObjects().object1;

		// Move camera
		m_game->setRequiredCameraPos( tmp->getPosition().y - 175 );

		// Check conditions for jump
		if(	((collision->getState() == CollisionState_Stay && !m_jump_key_pressed_in_last_frame) || collision->getState() == CollisionState_Start)
			&& 
			!m_isJump && IsKeyDown(m_jumpKey)
		)
		{
			m_isJump = true;
			m_forse.y += -m_heroJumpForse * 10000;

			if (tmp->getType() == GameObjectType_Platform)
			{
				if (((Platform*)tmp)->getPlatformState() & PlatformState_Broken)
					((Platform*)tmp)->setActive(false);
			}
		}
	}
	
}


void Hero::collisionEnter(Collision* collision)
{
	m_isJump = false;
	intersect(collision);
	GameObject::collisionEnter(collision);
}

void Hero::collisionStay(Collision* collision)
{
	intersect(collision);
	GameObject::collisionStay(collision);
}

void Hero::collisionExit(Collision* collision)
{
	GameObject::collisionExit(collision);
}

void Hero::setKeys(int leftKey, int rightKey, int fireKey)
{
	m_leftKey = leftKey;
	m_rightKey = rightKey;
	m_jumpKey = fireKey;
}