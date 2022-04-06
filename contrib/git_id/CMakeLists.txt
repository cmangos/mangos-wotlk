
cmake_minimum_required(VERSION 3.15)

add_executable(git_id git_id.cpp)

if(MSVC)
  # Define OutDir to source/bin/(platform)_(configuaration) folder.
  set_target_properties(git_id PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DEV_BIN_DIR}/git_id")
  set_target_properties(git_id PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DEV_BIN_DIR}/git_id")
  set_target_properties(git_id PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endif()
