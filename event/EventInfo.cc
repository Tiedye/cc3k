#include "EventInfo.h"
#include "EventTarget.h"
//#include <utility>
//
//using namespace std;
//
//EventInfo::EventInfo(EventInfo &&other) : event_position(other.event_position), primary{move(other.primary)}, secondary{move(other.secondary)}, secondaries{move(other.secondaries)} {}
EventInfo::Data::Data() {}

EventInfo::EventInfo() {}

EventInfo::EventInfo(const Position event_position) : event_position(event_position) {}

EventInfo::EventInfo(int event_integer, int event_integer2) : event_integer(event_integer), event_integer2(event_integer2) {}

EventInfo::EventInfo(double event_double) : event_double(event_double) {}

EventInfo::EventInfo(float event_float) : event_float(event_float) {}

EventInfo::EventInfo(EventInfo::Data *event_data_pointer) : event_data_pointer(event_data_pointer) {}
