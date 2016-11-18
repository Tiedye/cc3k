#pragma once
#include <list>
#include "EventType.h"

class EventTarget;

struct EventInfo {
	EventType eventType;
	EventTarget & getPrimary();
	EventTarget & getSecondary();
	std::list<EventTarget&> & getSecondaries();
};