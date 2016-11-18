#pragma once
#include <list>

class EventTarget;

class EventInfo {
public:
	EventTarget & getPrimary();
	EventTarget & getSecondary();
	std::list<EventTarget&> & getSecondaries();
};