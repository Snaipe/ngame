project (game CXX)

set (SOURCES
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
)

add_executable (game ${SOURCES})

target_link_libraries(game SDL2 SDL2_image)
