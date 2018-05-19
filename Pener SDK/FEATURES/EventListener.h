#pragma once

#include "../SDK/GameEvents.h"

#define CREATE_EVENT_LISTENER(class_name)\
class class_name : public SDK::IGameEventListener\
{\
public:\
	~class_name() { mopvar::GameEventManager->RemoveListener(this); }\
\
	virtual void FireGameEvent(SDK::IGameEvent* game_event);\
};\


namespace CGameEvents //fuck namespaces, fuck ur style bolbi
{
	void InitializeEventListeners();

	//CREATE_EVENT_LISTENER(ItemPurchaseListener);
	CREATE_EVENT_LISTENER(PlayerHurtListener);
	CREATE_EVENT_LISTENER(BulletImpactListener);
};
