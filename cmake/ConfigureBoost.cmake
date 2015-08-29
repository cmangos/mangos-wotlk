message("")

macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

        set(${version} "0x${ver}")
    endif()
endmacro()

if (WIN32)
  if (DEFINED BOOST_PATH)
    set(BOOST_ROOT ${BOOST_PATH})
  elseif (DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
  else()
    message(FATAL_ERROR "No BOOST_ROOT environment variable could be found! Please make sure it is set and the points to your Boost installation, or specify -DBOOST_PATH cmake option.")    
  endif()
	
  if (PLATFORM MATCHES X64 AND IS_DIRECTORY ${BOOST_ROOT}/stage/lib64)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib64)
  elseif (PLATFORM MATCHES X86 AND IS_DIRECTORY ${BOOST_ROOT}/stage/lib32)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib32)
  else()
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
  endif()
	  
  message(STATUS "Boost root: " ${BOOST_ROOT} " Boost library path: " ${BOOST_LIBRARYDIR})

  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME     OFF)

  get_WIN32_WINNT(ver)
  add_definitions(-D_WIN32_WINNT=${ver})
elseif (DEFINED BOOST_PATH)
  set(BOOST_LIBRARYDIR ${BOOST_PATH})
endif()

find_package(Boost 1.55 REQUIRED system thread program_options filesystem)
add_definitions(-DBOOST_DATE_TIME_NO_LIB)
add_definitions(-DBOOST_REGEX_NO_LIB)
add_definitions(-DBOOST_CHRONO_NO_LIB)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

message("")
