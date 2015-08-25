project (project_embb_tree_perf_test)

# Execute the GroupSources macro
include(${CMAKE_SOURCE_DIR}/CMakeCommon/GroupSourcesMSVC.cmake)
GroupSourcesMSVC(include)
GroupSourcesMSVC(test)

include_directories("include"
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../containers_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../containers_cpp/include)

file(GLOB_RECURSE PERF_HEADERS "include/*.h")

add_subdirectory(generator)
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/embb/containers/internal)

file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR})
add_custom_command (
  OUTPUT ${GENERATOR_OUTPUT_DIR}/fgl_chromatic_tree-rebalance.h
  COMMAND fgl_generator ${GENERATOR_OUTPUT_DIR}/fgl_chromatic_tree-rebalance.h
  DEPENDS fgl_generator
)
add_custom_command (
  OUTPUT ${GENERATOR_OUTPUT_DIR}/cgl_chromatic_tree-rebalance.h
  COMMAND cgl_generator ${GENERATOR_OUTPUT_DIR}/cgl_chromatic_tree-rebalance.h
  DEPENDS cgl_generator
)

set(GENERATED_HEADERS ${GENERATOR_OUTPUT_DIR}/fgl_chromatic_tree-rebalance.h
                      ${GENERATOR_OUTPUT_DIR}/cgl_chromatic_tree-rebalance.h)

add_executable(tree_perf main.cc ${PERF_HEADERS} ${GENERATED_HEADERS})
target_link_libraries(tree_perf embb_containers_cpp embb_base_cpp
                                embb_base_c ${compiler_libs})
if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(tree_perf PUBLIC -std=c++11)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../partest/include)
add_executable(fglcgl_test test/main.cc ${GENERATED_HEADERS})
target_link_libraries(fglcgl_test embb_containers_cpp partest embb_base_cpp
                                  embb_base_c ${compiler_libs})
