project (game CXX)

set (SOURCES
  src/main.cc
  src/metaball.cc
  src/metaball.h
)

add_executable (game ${SOURCES})

target_link_libraries(game SDL2)
