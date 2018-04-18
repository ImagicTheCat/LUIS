cmake_minimum_required(VERSION 3.0)

project(luis)

set(HS
  # dependencies
  ../sub/ed25519/src/precomp_data.h
  ../sub/ed25519/src/fe.h
  ../sub/ed25519/src/ge.h
  ../sub/ed25519/src/sc.h
  ../sub/ed25519/src/sha512.h
  ../sub/ed25519/src/fixedint.h
  ../sub/ed25519/src/ed25519.h

  # main
  ../src/submains.hpp
  ../src/network.hpp
  ../src/MainArgs.hpp
  ../src/MapData.hpp
)

set(SS
  # dependencies
  ../sub/ed25519/src/seed.c
  ../sub/ed25519/src/sc.c
  ../sub/ed25519/src/add_scalar.c
  ../sub/ed25519/src/ge.c
  ../sub/ed25519/src/keypair.c
  ../sub/ed25519/src/verify.c
  ../sub/ed25519/src/key_exchange.c
  ../sub/ed25519/src/sign.c
  ../sub/ed25519/src/fe.c
  ../sub/ed25519/src/sha512.c

  # main
  ../src/MainArgs.cpp
  ../src/MapData.cpp
  ../src/submain_server.cpp
  ../src/submain_list.cpp
  ../src/main.cpp
)

if(WIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
  set(LIBS
    ws2_32 Shlwapi)

  # no Release/Debug dirs on windows
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
elseif(UNIX)
  set(LIBS
    )
endif()

add_executable(luis ${SS} ${HS})
target_link_libraries(luis ${LIBS})