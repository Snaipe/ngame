cmake_minimum_required (VERSION 2.8.11)

project (game CXX)

set (SOURCES
  src/ai.cc
  src/ai.h
  src/camera.cc
  src/camera.h
  src/drawable.h
  src/event.cc
  src/event.h
  src/event.hxx
  src/level.cc
  src/level.h
  src/main.cc
  src/metaball.cc
  src/metaball.h
  src/textures.cc
  src/textures.h
  src/tickable.h
  src/ui.cc
  src/ui.h
  src/genetics.cc
  src/genetics.h
  src/colors.cc
  src/colors.h
)

if (NOT WIN32)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
  add_definitions(-DNDEBUG)
endif ()

add_executable (game ${SOURCES})

target_link_libraries(game SDL2 SDL2_image)
