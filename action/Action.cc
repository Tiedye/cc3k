#include "Action.h"

Action::Action(EventType preEvent, EventType postEvent, Action::Targets targets, int range, Action::Shape shape,
               bool aoe, int aoeSize) : preEvent{preEvent}, postEvent{postEvent}, targets{targets}, range{range},
                                        shape{shape}, aoe{aoe}, aoeSize{aoeSize} {

}
