
cmake_minimum_required(VERSION 2.8)

include(../../cmake/common.cmake)
include(CheckPlatform)

add_executable(git_id git_id.cpp)

if(WIN32 AND MSVC)
  if(PLATFORM MATCHES X86) # 32-bit
    set(FOLDER_ARCH x86)
  else() # 64-bit
    set(FOLDER_ARCH x64)
  endif()

  # Define OutDir to source/bin/(platform)_(configuaration) folder.
  set_target_properties(git_id PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DEV_BIN_DIR}/git_id")
  set_target_properties(git_id PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DEV_BIN_DIR}/git_id")
  set_target_properties(git_id PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")

  # Define windows application type to not see a console
  set_target_properties(git_id PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()
