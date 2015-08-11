project (project_embb_tree_perf_test)

include_directories("include"
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_c/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_c/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../base_cpp/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../containers_cpp/include
                    ${CMAKE_CURRENT_BINARY_DIR}/../containers_cpp/include)

file(GLOB_RECURSE PERF_HEADERS "include/*.h")

add_executable(tree_perf main.cc ${PERF_HEADERS})
if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(tree_perf PUBLIC -std=c++11)
endif()
target_link_libraries(tree_perf embb_containers_cpp embb_base_cpp
                                embb_base_c ${compiler_libs})
