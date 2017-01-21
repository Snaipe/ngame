project (game CXX)

set (SOURCES
  src/main.cc
)

add_executable (game ${SOURCES})

target_link_libraries(game SDL2)
