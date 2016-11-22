#pragma once

#include "util/EventType.h"
#include "EventInfo.h"

class Listener {
	//static int listenersCreated {0};
public:
    //const int id {listenersCreated++};
	virtual ~Listener() = default;
	virtual void notify(EventInfo &info) = 0;
	virtual const std::vector<EventType> listeningFor() const = 0;
};