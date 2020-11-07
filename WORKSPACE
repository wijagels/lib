load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_cc",
    sha256 = "11cb7eee61da48330fa02f110930f0924ebd2058f6f95f3c86020a6c814daae2",
    strip_prefix = "rules_cc-991eb349bf648b89f7d039a2602d337c85479534",
    urls = [
        "https://github.com/bazelbuild/rules_cc/archive/991eb349bf648b89f7d039a2602d337c85479534.tar.gz",
    ],
)

http_archive(
    name = "com_google_googletest",
    sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
    strip_prefix = "googletest-release-1.10.0",
    urls = [
        "https://github.com/google/googletest/archive/release-1.10.0.tar.gz",
    ],
)

http_archive(
    name = "com_github_google_benchmark",
    sha256 = "dccbdab796baa1043f04982147e67bb6e118fe610da2c65f88912d73987e700c",
    strip_prefix = "benchmark-1.5.2",
    urls = [
        "https://github.com/google/benchmark/archive/v1.5.2.tar.gz",
    ],
)

http_archive(
    name = "com_google_absl",
    sha256 = "f41868f7a938605c92936230081175d1eae87f6ea2c248f41077c8f88316f111",
    strip_prefix = "abseil-cpp-20200225.2",
    urls = [
        "https://github.com/abseil/abseil-cpp/archive/20200225.2.tar.gz",
    ],
)

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "0a1d884aa13201b705f93b86d2d0be1de867f6e592de3c4a3bbe6d04bdddf593",
    strip_prefix = "rules_boost-a32cad61d9166d28ed86d0e07c0d9bca8db9cb82",
    urls = [
        "https://github.com/nelhage/rules_boost/archive/a32cad61d9166d28ed86d0e07c0d9bca8db9cb82.tar.gz",
    ],
)

http_archive(
    name = "boost",
    build_file = "@com_github_nelhage_rules_boost//:BUILD.boost",
    patch_cmds = ["rm -f doc/pdf/BUILD"],
    sha256 = "afff36d392885120bcac079148c177d1f6f7730ec3d47233aa51b0afa4db94a5",
    strip_prefix = "boost_1_74_0",
    urls = [
        # "https://mirror.bazel.build/dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz",
        "https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz",
    ],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
    ],
)
