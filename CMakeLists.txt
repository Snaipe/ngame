cmake_minimum_required (VERSION 2.8.11)

project (game CXX)

set (SOURCES
  src/ai.cc
  src/ai.h
  src/camera.cc
  src/camera.h
  src/colors.cc
  src/colors.h
  src/drawable.h
  src/event.cc
  src/event.h
  src/event.hxx
  src/genetics.cc
  src/genetics.h
  src/level.cc
  src/level.h
  src/main.cc
  src/metaball.cc
  src/metaball.h
  src/screens.cc
  src/screens.h
  src/textures.cc
  src/textures.h
  src/tickable.h
  src/ui-title.cc
  src/ui-title.h
  src/ui.cc
  src/ui.h
)

add_definitions(-DSDL_MAIN_HANDLED)

if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
  add_definitions(-DNDEBUG)
endif ()

add_executable (game ${SOURCES})

set_target_properties(game PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(game PROPERTIES LINK_SEARCH_END_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

target_link_libraries(game SDL2 SDL2_image)
