#pragma once
#include <vector>
#include <memory>
#include "EventType.h"
#include "../util/Position.h"

class EventTarget;

struct EventInfo {
    EventInfo();
    //EventInfo(EventInfo &&other);
    struct Data{
        Data();

        union {
            Position position;
            struct {
                int integer1;
                int integer2;
            };
			struct {
				short short1;
				short short2;
				short short3;
				short short4;
			};
            double doubleFloat;
            struct {
                float float1;
                float float2;
            };
        };
    };
    union {
        Position eventPosition;
		struct {
			int eventInteger;
			int eventInteger2;
		};
		double eventDouble;
		float eventFloat;
		Data *eventDataPointer;
    };
	EventType eventType;
	std::unique_ptr<EventTarget> primary;
    std::unique_ptr<EventTarget> secondary;
	std::vector<std::unique_ptr<EventTarget>> secondaries;
};



