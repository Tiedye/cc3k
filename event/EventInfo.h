#pragma once
#include <vector>
#include <memory>
#include "EventType.h"
#include "../util/Position.h"

class EventTarget;

struct EventInfo {
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

    EventInfo(const Position eventPosition);
    EventInfo(int eventInteger, int eventInteger2);
    EventInfo(double eventDouble);
    EventInfo(float eventFloat);
    EventInfo(EventInfo::Data *eventDataPointer);
    EventInfo();
    //EventInfo(EventInfo &&other);
    union {
        const Position eventPosition;
		struct {
			const int eventInteger;
			const int eventInteger2;
		};
		const double eventDouble;
		const float eventFloat;
		Data * const eventDataPointer;
    };
	EventType eventType {EventType::GENERIC};
	std::unique_ptr<EventTarget> primary;
    std::unique_ptr<EventTarget> secondary;
	std::vector<std::unique_ptr<EventTarget>> secondaries;
};



