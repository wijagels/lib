find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

add_executable(list_test list_test.cpp)
target_link_libraries(list_test ${Boost_LIBRARIES})

add_executable(decorator_test decorator_test.cpp)
target_link_libraries(decorator_test ${Boost_LIBRARIES})

add_executable(point_test point_test.cpp)
target_link_libraries(point_test ${Boost_LIBRARIES})

add_executable(skiplist_test skiplist_test.cpp)
target_link_libraries(skiplist_test ${Boost_LIBRARIES})

add_executable(map_test map_test.cpp)
target_link_libraries(map_test ${Boost_LIBRARIES})

add_executable(vector_test vector_test.cpp)
target_link_libraries(vector_test ${Boost_LIBRARIES})

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
endif()
if(CLANG_TIDY_EXE)
  set_target_properties(
      map_test PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
  set_target_properties(
      list_test PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
  set_target_properties(
      skiplist_test PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
  set_target_properties(
      point_test PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
  set_target_properties(
      vector_test PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()
