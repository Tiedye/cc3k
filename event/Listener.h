#pragma once

#include "EventType.h"
#include "EventInfo.h"

class Listener {
	//static int listeners_created {0};
public:
	Listener();
	Listener(const int priority);
	//const int id {listeners_created++};
	virtual ~Listener() = default;
	virtual void notify(EventInfo &info) = 0;
	virtual const std::vector<EventType> listening_for() const = 0;
	const int priority;
};