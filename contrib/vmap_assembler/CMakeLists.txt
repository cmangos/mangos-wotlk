# This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

cmake_minimum_required (VERSION 2.6)
set(EXECUTABLE_NAME "vmap_assembler")
project (${EXECUTABLE_NAME})

# uncomment next line to disable debug mode
if(DEBUG)
    ADD_DEFINITIONS("-DIOMAP_DEBUG")
endif()

ADD_DEFINITIONS("-DNO_CORE_FUNCS")

ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-ggdb")
ADD_DEFINITIONS("-O3")

include_directories(../../src/shared)
include_directories(../../src/game/vmap/)
include_directories(../../dep/include/g3dlite/)
include_directories(../../src/framework/)

add_library(vmap
    ../../src/game/vmap/BIH.cpp
    ../../src/game/vmap/VMapManager2.cpp
    ../../src/game/vmap/MapTree.cpp
    ../../src/game/vmap/TileAssembler.cpp
    ../../src/game/vmap/WorldModel.cpp
    ../../src/game/vmap/ModelInstance.cpp
    )

target_link_libraries(vmap g3dlite z)

add_executable(${EXECUTABLE_NAME} vmap_assembler.cpp)
target_link_libraries(${EXECUTABLE_NAME} vmap)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${BIN_DIR}/tools)

# add_executable(vmap_test coordinate_test.cpp)
# target_link_libraries(vmap_test vmap)

