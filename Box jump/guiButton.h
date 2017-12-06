#pragma once
////////////////////////////////////////////////
// Include
#include "guiText.h"
#include "guiSprite.h"

////////////////////////////////////////////////
// Forvard declaration
class EventController;
enum GameEvents;

////////////////////////////////////////////////
// Class GUIButton
class GUIButton : public GUIText, public GUISprite
{
public:
	GUIButton();

	virtual void render();
	virtual void update();

	void setEvent(GameEvents gameEvent) { m_event = gameEvent; };
	void setEventController(EventController* controller) { m_eventController = controller; };

private:
	GameEvents m_event;
	EventController* m_eventController;
};