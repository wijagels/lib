find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

add_executable(list_test list_test.cpp)
target_link_libraries(list_test ${Boost_LIBRARIES} Threads::Threads)

add_executable(decorator_test decorator_test.cpp)
target_link_libraries(decorator_test ${Boost_LIBRARIES})

add_executable(point_test point_test.cpp)
target_link_libraries(point_test ${Boost_LIBRARIES})

add_executable(skiplist_test skiplist_test.cpp)
target_link_libraries(skiplist_test ${Boost_LIBRARIES})
