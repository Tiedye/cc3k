cmake_minimum_required(VERSION 3.6)
project(cc3k)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

set(SOURCE_FILES main.cc entity/Character.cc entity/Character.h entity/Entity.cc entity/Entity.h EventInfo.h EventTarget.cc EventTarget.h util/EventType.h Listener.h util/Size.h entity/Item.cc entity/Item.h stage/Dungeon.cc stage/Dungeon.h CellType.h stage/Stage.cc stage/Stage.h Game.cc Game.h State.cc State.h controller/Controller.cc controller/Controller.h display/DungeonRenderer.cc display/DungeonRenderer.h util/Position.h display/ConsoleDungeonRenderer.cc display/ConsoleDungeonRenderer.h LifeSteal.cc LifeSteal.h entity/Equippable.cc entity/Equippable.h FeatureSet.cc FeatureSet.h StatModifier.h Action.cc Action.h util/Direction.h util/Position.cc Listener.cc util/StatType.h entity/Stat.cc entity/Stat.h util/EffectType.h controller/Command.cc controller/Command.h entity/Consumable.cc entity/Consumable.h stage/Menu.cc stage/Menu.h loader/DungeonLoader.cc loader/DungeonLoader.h stage/EndScreen.cc stage/EndScreen.h loader/Loader.cc loader/Loader.h Race.cc Race.h StatModifier.cc EventInfo.cc)
add_executable(cc3k ${SOURCE_FILES})