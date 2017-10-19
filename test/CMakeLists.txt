find_package(GTest REQUIRED)

function(register_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} PRIVATE Wijagels::lib GTest::GTest GTest::Main)
    add_test(${test_name} ${test_name})
endfunction(register_test)

register_test(list_test list_test.cpp)
register_test(decorator_test decorator_test.cpp)
register_test(point_test point_test.cpp)
register_test(skiplist_test skiplist_test.cpp)
register_test(map_test map_test.cpp)
register_test(vector_test vector_test.cpp)
register_test(algorithm_test algorithm_test.cpp)

find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
  )
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
  set_target_properties(
      map_test
      list_test
      skiplist_test
      point_test
      vector_test
      algorithm_test
    PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
      LINK_WHAT_YOU_USE TRUE
  )
endif()
