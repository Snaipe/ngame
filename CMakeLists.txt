project (game CXX)

set (SOURCES
  src/ai.cc
  src/ai.h
  src/camera.cc
  src/camera.h
  src/drawable.h
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
)

if (NOT WIN32)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif ()

add_executable (game ${SOURCES})

target_link_libraries(game SDL2 SDL2_image)
