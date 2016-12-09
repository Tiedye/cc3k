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
            double double_float;
            struct {
                float float1;
                float float2;
            };
        };
    };

    EventInfo(const Position event_position);
    EventInfo(int event_integer, int event_integer2);
    EventInfo(double event_double);
    EventInfo(float event_float);
    EventInfo(EventInfo::Data *event_data_pointer);
    EventInfo();
    //EventInfo(EventInfo &&other);
    union {
        const Position event_position;
		struct {
			const int event_integer;
			const int event_integer2;
		};
		const double event_double;
		const float event_float;
		Data * const event_data_pointer;
    };
	EventType event_type {EventType::GENERIC};
	std::unique_ptr<EventTarget> primary;
    std::unique_ptr<EventTarget> secondary;
	std::vector<std::unique_ptr<EventTarget>> secondaries;
};



