project (project_embb_da_tools)

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../containers_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../containers_cpp/include)

# DLL for the CHESS tool
if(MSVC OR CYGWIN)
  add_library(ct_chess SHARED ct_chess.cc dynamic_analysis_test.h)
  target_link_libraries(ct_chess embb_containers_cpp embb_base_cpp
                                 embb_base_c ${compiler_libs})
  target_compile_definitions(ct_chess PRIVATE CT_CHESS_DLL)
endif()

# Executable for Maple tool
add_executable(ct_maple ct_maple.cc dynamic_analysis_test.h)
target_link_libraries(ct_maple embb_containers_cpp embb_base_cpp
                               embb_base_c ${compiler_libs})
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(ct_maple PRIVATE -fno-omit-frame-pointer)
endif()
