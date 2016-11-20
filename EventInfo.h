#pragma once
#include <vector>
#include <memory>
#include "util/EventType.h"
#include "util/Position.h"

class EventTarget;

struct EventInfo {
    union {
        Position eventPosition;
        int eventInteger;
        double eventDouble;
    };
	EventType eventType;
	std::unique_ptr<EventTarget> primary;
    std::unique_ptr<EventTarget> secondary;
	std::vector<std::unique_ptr<EventTarget>> secondaries;
};