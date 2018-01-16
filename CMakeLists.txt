cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=5 -Wundef -Wswitch-default)
# add_compile_options(-fmodule-map-file=${CMAKE_CURRENT_SOURCE_DIR}/modules.map -fimplicit-modules -fmodules-ts)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -fno-omit-frame-pointer")

find_package(Boost REQUIRED)

add_library(lib INTERFACE)
add_library(Wijagels::lib ALIAS lib)
target_include_directories(lib INTERFACE include)
target_link_libraries(lib INTERFACE Boost::boost)

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)
