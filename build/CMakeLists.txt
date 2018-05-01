cmake_minimum_required(VERSION 3.0)

project(luis)

set(HS
  # main
  ../src/submains.hpp
  ../src/network.hpp
  ../src/Command.hpp
  ../src/MainArgs.hpp
  ../src/MapData.hpp
  ../src/Dir.hpp
  ../src/Contract.hpp
  ../src/tools.hpp
)

set(SS
  # main
  ../src/MainArgs.cpp
  ../src/MapData.cpp
  ../src/Dir.cpp
  ../src/Contract.cpp
  ../src/tools.cpp
  ../src/submain_server.cpp
  ../src/submain_identity.cpp
  ../src/submain_key.cpp
  ../src/main.cpp
)

if(WIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
  set(LIBS
    ws2_32 Shlwapi sodium)

  # no Release/Debug dirs on windows
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
elseif(UNIX)
  set(LIBS
    sodium
    )
endif()

add_executable(luis ${SS} ${HS})
target_link_libraries(luis ${LIBS})
