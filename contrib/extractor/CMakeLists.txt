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
set(EXECUTABLE_NAME "ad")
project (${EXECUTABLE_NAME})

add_subdirectory (libmpq)
add_subdirectory (loadlib)

include_directories (${MANGOS_MAP_EXTRACTOR_SOURCE_DIR}/libmpq)
include_directories (${MANGOS_MAP_EXTRACTOR_SOURCE_DIR}/loadlib)

link_directories (${MANGOS_MAP_EXTRACTOR_SOURCE_DIR}/libmpq)
link_directories (${MANGOS_MAP_EXTRACTOR_SOURCE_DIR}/loadlib)

add_executable (${EXECUTABLE_NAME} dbcfile.cpp mpq_libmpq.cpp System.cpp)

target_link_libraries (${EXECUTABLE_NAME} libmpq)
target_link_libraries (${EXECUTABLE_NAME} loadlib)

install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${BIN_DIR}/tools)
