cmake_minimum_required(VERSION 3.0)

project(lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported _flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${_flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${_flag}" _flag_supported)
        if(_flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}" PARENT_SCOPE)
        else()
            message(STATUS "Unsupported _flag ${_flag}")
        endif()
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-Werror")
enable_cxx_compiler_flag_if_supported("-pedantic")
# enable_cxx_compiler_flag_if_supported("-O2")
enable_cxx_compiler_flag_if_supported("-g")
enable_cxx_compiler_flag_if_supported("-Wno-gnu-statement-expression")
# enable_cxx_compiler_flag_if_supported("-Wlogical-op")
# enable_cxx_compiler_flag_if_supported("-Wstrict-null-sentinel")
# enable_cxx_compiler_flag_if_supported("-Wnoexcept")
set(CXXFLAGS "-Winline -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
separate_arguments(CXXFLAGS)
list(LENGTH CXXFLAGS len)
foreach(flag IN LISTS CXXFLAGS)
    enable_cxx_compiler_flag_if_supported("${flag}")
endforeach(flag)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
enable_testing()
add_test(ListTest test/list_test)
add_test(DecoratorTest test/decorator_test)
add_test(PointTest test/point_test)
add_test(SkipListTest test/skiplist_test)
add_test(MapTest test/map_test)
add_test(VectorTest test/vector_test)
