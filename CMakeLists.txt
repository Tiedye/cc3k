cmake_minimum_required(VERSION 3.6)
project(cc3k)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

set(SOURCE_FILES main.cc Character.cc Character.h Entity.cc Entity.h EventInfo.h EventTarget.cc EventTarget.h EventType.h Listener.h Size.h Item.cpp Item.h Dungeon.cpp Dungeon.h CellType.h Stage.cpp Stage.h Game.cpp Game.h State.cpp State.h Controller.cpp Controller.h)
add_executable(cc3k ${SOURCE_FILES})