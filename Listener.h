#pragma once

#include "EventType.h"
#include "EventInfo.h"

class Listener {
public:
	virtual ~Listener() = default;
	virtual void notify( EventInfo info ) = 0;
	virtual EventType listeningFor() = 0;
};
