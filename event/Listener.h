#pragma once

#include "EventType.h"
#include "EventInfo.h"

class Listener {
	//static int listenersCreated {0};
public:
	Listener();
	Listener(const int priority);
	//const int id {listenersCreated++};
	virtual ~Listener() = default;
	virtual void notify(EventInfo &info) = 0;
	virtual const std::vector<EventType> listeningFor() const = 0;
	const int priority;
};