#include "EventInfo.h"
#include "EventTarget.h"
//#include <utility>
//
//using namespace std;
//
//EventInfo::EventInfo(EventInfo &&other) : eventPosition(other.eventPosition), primary{move(other.primary)}, secondary{move(other.secondary)}, secondaries{move(other.secondaries)} {}
EventInfo::Data::Data() {}

EventInfo::EventInfo() {}

EventInfo::EventInfo(const Position eventPosition) : eventPosition(eventPosition) {}

EventInfo::EventInfo(int eventInteger, int eventInteger2) : eventInteger(eventInteger), eventInteger2(eventInteger2) {}

EventInfo::EventInfo(double eventDouble) : eventDouble(eventDouble) {}

EventInfo::EventInfo(float eventFloat) : eventFloat(eventFloat) {}

EventInfo::EventInfo(EventInfo::Data *eventDataPointer) : eventDataPointer(eventDataPointer) {}
