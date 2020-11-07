load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "map",
    hdrs = [
        "include/Map.hpp",
    ],
    strip_include_prefix = "include",
    deps = [":skiplist"],
)

cc_library(
    name = "skiplist",
    hdrs = [
        "include/SkipList.hpp",
    ],
    strip_include_prefix = "include",
    deps = ["@boost//:container"],
)

cc_library(
    name = "list",
    hdrs = [
        "include/List.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "allocator",
    hdrs = [
        "include/Allocator.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "algorithm",
    hdrs = [
        "include/algorithm.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "decorators",
    hdrs = [
        "include/Decorators.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "function",
    hdrs = [
        "include/Function.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "point",
    hdrs = [
        "include/Point.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "stack",
    hdrs = [
        "include/Stack.hpp",
    ],
    strip_include_prefix = "include",
    deps = [":list"],
)

cc_library(
    name = "vector",
    hdrs = [
        "include/Vector.hpp",
    ],
    strip_include_prefix = "include",
)
