#pragma once
#include <vector>
#include <memory>
#include "util/EventType.h"
#include "util/Position.h"

class EventTarget;

struct EventInfo {
    //EventInfo(EventInfo &&other);
    struct Data{
        union {
            Position position;
            int integer1;
            int integer2;
            double doubleFloat;
            float float1;
            float float2;
        };
    };
    union {
        Position eventPosition;
        int eventInteger;
        int eventInteger2;
		double eventDouble;
		float eventFloat;
		Data *eventDataPointer;
    };
	EventType eventType;
	std::unique_ptr<EventTarget> primary;
    std::unique_ptr<EventTarget> secondary;
	std::vector<std::unique_ptr<EventTarget>> secondaries;
};



