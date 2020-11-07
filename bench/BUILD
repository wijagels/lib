cc_test(
    name = "skiplist",
    srcs = ["skiplist_bench.cpp"],
    deps = [
        "//:skiplist",
        "@com_github_google_benchmark//:benchmark_main",
        "@boost//:assert",
        "@boost//:pool",
        "@boost//:throw_exception",
        "@boost//:type_traits",
    ],
    tags = ["benchmark"],
)
