load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "skiplist",
    srcs = [
        "skiplist_test.cpp",
    ],
    deps = [
        "//:skiplist",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "list",
    srcs = [
        "list_test.cpp",
    ],
    deps = [
        "//:allocator",
        "//:list",
        "@boost//:assert",
        "@boost//:pool",
        "@boost//:throw_exception",
        "@boost//:type_traits",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "algorithm",
    srcs = [
        "algorithm_test.cpp",
    ],
    linkopts = ["-ltbb"],
    deps = [
        "//:algorithm",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "decorator",
    srcs = [
        "decorator_test.cpp",
    ],
    deps = [
        "//:decorators",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "map",
    srcs = [
        "map_test.cpp",
    ],
    deps = [
        "//:list",
        "//:map",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "point",
    srcs = [
        "point_test.cpp",
    ],
    deps = [
        "//:point",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "vector",
    srcs = [
        "vector_test.cpp",
    ],
    deps = [
        "//:vector",
        "@com_google_googletest//:gtest_main",
    ],
)
