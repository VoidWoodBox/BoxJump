/////////////////////////////////////////////////
// Include
#include "interface\interface.h"
#include "interface\interfaceWindow.h"
#include "interface\uiButton.h"
#include "interface\uiText.h"
#include "interface\uiSprite.h"

#include "game.h"
#include "render/render.h"
#include "dataManager\dataManager.h"
#include "dataManager\levelSettings.h"
#include "eventSystem\eventController.h"
#include "inputController.h"

#include "gameObject\hero.h"

#include "tools/utils.h"

/////////////////////////////////////////////////
// Variables
static MenuType activeWindowIndex = MenuType_MainMenu;
static float cloudSpeed;
static float deltaTime;
static UIText* winnerCongratulation = 0;
static int scale = 0;

/////////////////////////////////////////////////
// Functions
std::string getMenuNameByType(MenuType type)
{
	switch (type) {
		case MenuType_MainMenu:			return "Main";
		case MenuType_InGameMenu:		return "In-GameMenu";
		case MenuType_PauseMenu:		return "PauseMenu";
		case MenuType_CustomizeMenu:	return "CustomizeMenu";
		case MenuType_ScoreMenu:		return "ScoreMenu";
		case MenuType_StatisticWindow:	return "Statistic";
		case MenuType_Settings:			return "Settings";
	}

	return "";
}

/////////////////////////////////////////////////
// Class Interface

Interface::~Interface()
{
	for (int i = 0; i < MenuTypeCount; i++)
	{
		if (m_menuList[i] != 0)
		{
			delete m_menuList[i];
			m_menuList[i] = 0;
		}
	}
}

void Interface::setupInterface()
{
	m_game = Game::instance();
	m_render = Render::instance();
	m_eventController = EventController::instance();
	m_dataManager = DataManager::instance();

	m_heroSkin = (HeroSkin)m_dataManager->getObjectsSettgins()->get<int>("Hero.Skin");
	m_player1Skin = (HeroSkin)m_dataManager->getObjectsSettgins()->get<int>("Player1.Skin");
	m_player2Skin = (HeroSkin)m_dataManager->getObjectsSettgins()->get<int>("Player2.Skin");

	startListeningEvents();

	for (int i = 0; i < MenuTypeCount; i++)
	{
		m_menuList[i] = 0;
	}

	deltaTime = 1. / m_dataManager->getWindowSettings()->get<int>("Framerate", 60);
	cloudSpeed = m_dataManager->getObjectsSettgins()->get<float>("Cloud.CloudSpeed");
}

void Interface::startListeningEvents() {
	m_eventController->addListenerToEvent(
		this,
		"OnStart1pGameButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_InGameMenu; winnerCongratulation->setString(""); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnStart2pGameButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_InGameMenu; winnerCongratulation->setString(""); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnStartButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_InGameMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnPauseButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_PauseMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnMainMenuButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_MainMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnCustomizeButtonDown",
		[](const EventListener* listener) {activeWindowIndex = MenuType_CustomizeMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnGameEnd",
		[](const EventListener* listener) { activeWindowIndex = MenuType_ScoreMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnRestartButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_InGameMenu; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnStatisticButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_StatisticWindow; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnSettingsButtonDown",
		[](const EventListener* listener) { activeWindowIndex = MenuType_Settings; }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnFirstPlayerWin",
		[](const EventListener* listener) { winnerCongratulation->setString("First player won"); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnSecondPlayerWin",
		[](const EventListener* listener) { winnerCongratulation->setString("Second player won"); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnLeftArrowSpDown",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Hero", -1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnRightArrowSpDown",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Hero", 1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnLeftArrowP1Down",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Player1", -1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnRightArrowP1Down",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Player1", 1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnLeftArrowP2Down",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Player2", -1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnRightArrowP2Down",
		[](const EventListener* listener) { ((Interface*)listener)->changeSprite("Player2", 1); }
	);

	m_eventController->addListenerToEvent(
		this,
		"OnChangeRosolutionButtonDown",
		[](const EventListener* listener) {((Interface*)listener)->changeToNextResolution(); }
	);
}

void Interface::changeToNextResolution() {
	scale = (scale + 1) % Resolution_Count;
	if (scale == 0) scale++;

	int width, height; 
	getResolution((Resolutions)scale, width, height);

	m_resolution->setString("" + std::to_string(width) + "x" + std::to_string(height));
}

void Interface::changeSprite(std::string name, int change){
	if (name == "Hero") {
		m_heroSkin = (HeroSkin)((m_heroSkin + HeroSkin_Count + change) % HeroSkin_Count);
		m_heroSprite->resetSprite(getSpriteNameBySkin(m_heroSkin));
	}

	if (name == "Player1") {
		m_player1Skin = (HeroSkin)((m_player1Skin + HeroSkin_Count + change) % HeroSkin_Count);
		m_player1Sprite->resetSprite(getSpriteNameBySkin(m_player1Skin));
	}

	if (name == "Player2") {
		m_player2Skin = (HeroSkin)((m_player2Skin + HeroSkin_Count + change) % HeroSkin_Count);
		m_player2Sprite->resetSprite(getSpriteNameBySkin(m_player2Skin));
	}
}

void Interface::initialize() {
	for (int i = 0; i < MenuTypeCount; i++) {
		std::string menuName = getMenuNameByType((MenuType)i);

		m_menuList[i] = new InterfaceWindow();
		m_menuList[i]->setCamera(m_render->getCamera(CameraType_Main));
		m_menuList[i]->initialize(menuName);
	}

	scale = m_render->getScale() - 1;

	m_resolution		 =  (UIText*) m_menuList[MenuType_Settings]->getObject("Resolution");
	winnerCongratulation =  (UIText*) m_menuList[MenuType_ScoreMenu]->getObject("WinnerCongratulation");
	m_heroSprite		 = (UISprite*)m_menuList[MenuType_CustomizeMenu]->getObject("HeroSprite");
	m_player1Sprite		 = (UISprite*)m_menuList[MenuType_CustomizeMenu]->getObject("P1Sprite");
	m_player2Sprite		 = (UISprite*)m_menuList[MenuType_CustomizeMenu]->getObject("P2Sprite");
	m_cloudSprite		 = (UISprite*)m_menuList[MenuType_MainMenu]->getObject("Cloud");

	m_heroSprite->resetSprite(getSpriteNameBySkin(m_heroSkin));
	m_player1Sprite->resetSprite(getSpriteNameBySkin(m_player1Skin));
	m_player2Sprite->resetSprite(getSpriteNameBySkin(m_player2Skin));

	changeToNextResolution();
}

void Interface::changeWindow(MenuType windowType) {
	m_menuList[activeWindowIndex]->setActive(false);
	activeWindowIndex = windowType;
	m_menuList[activeWindowIndex]->setActive(true);
}

void Interface::update() {
	changeWindow(activeWindowIndex);

	m_menuList[activeWindowIndex]->update();

	if (activeWindowIndex == MenuType_MainMenu) {
		m_cloudSprite->addToPosition(cloudSpeed*deltaTime, 0);

		if (m_cloudSprite->getPosition().x > kScreenWidth / 1.8f) {
			m_cloudSprite->setPosition(-kScreenWidth / 2 - GetRandomFloat(32, 85), GetRandomInt(-kScreenHeight / 3, kScreenHeight / 3));

			sf::IntRect rect = m_dataManager->getSpriteContainer("CloudSprite")->rect;
			rect.top += GetRandomInt(0, 2) * 16;
			m_cloudSprite->setRect(rect);
		}
	}
}

void Interface::render() {
	m_menuList[activeWindowIndex]->render();
}

InterfaceWindow* Interface::createWindow(MenuType type) {
	m_menuList[type] = new InterfaceWindow();
	m_menuList[type]->setActive(true);

	return m_menuList[type];
}