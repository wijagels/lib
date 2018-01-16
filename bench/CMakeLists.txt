find_package(benchmark)

add_executable(skiplist_bench skiplist_bench.cpp)
target_link_libraries(skiplist_bench PUBLIC benchmark Wijagels::lib)
