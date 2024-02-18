# Since it's a Golang library (https://github.com/walkline/ToCloud9/tree/master/game-server/libsidecar), to
# build a C library from Go sources on Windows, you need to use the GCC compiler. However, cmangos is
# configured to use the MSVC compiler or clang-cl, and both of them are not compatible with GCC on Windows.
#
# This can be changed with implementation of one of these feature requests:
#   https://github.com/golang/go/issues/17014
#   https://github.com/golang/go/issues/20982

if (WIN32 OR NOT USE_REAL_LIBSIDECAR)

    file(GLOB sources stub/*.c stub/*.h)

    add_library(libsidecar STATIC ${sources})

    set_target_properties(libsidecar 
      PROPERTIES 
        LINKER_LANGUAGE 
          CXX
        INTERFACE_INCLUDE_DIRECTORIES
          ${CMAKE_CURRENT_SOURCE_DIR}/stub)

else()
    add_library(libsidecar SHARED IMPORTED GLOBAL)

    set_target_properties(libsidecar
      PROPERTIES
        IMPORTED_LOCATION
          ${CMAKE_CURRENT_SOURCE_DIR}/libsidecar.so
        INTERFACE_INCLUDE_DIRECTORIES
          ${CMAKE_CURRENT_SOURCE_DIR}/include
        IMPORTED_NO_SONAME true)

    set_target_properties(libsidecar PROPERTIES
                          IMPORTED_LOCATION_NOCONFIG  "${CMAKE_INSTALL_RPATH}/libsidecar.so"
                          IMPORTED_SONAME_NOCONFIG libsidecar.so
    )
endif()