vars = {
  "chromium_git": "https://chromium.googlesource.com",
}

deps = {
  # chromium's buildtools containing libc++, libc++abi, clang_format and gn.
  "ppi/buildtools": "https://chromium.googlesource.com/chromium/buildtools@24ebce4578745db15274e180da1938ebc1358243",

  # gflags 2.2.1
  "ppi/third_party/gflags/gflags": "https://github.com/gflags/gflags.git@46f73f88b18aee341538c0dfc22b1710a6abedef",

  # google-glog
  "ppi/third_party/glog/glog": "https://github.com/google/glog.git@2063b387080c1e7adffd33ca07adff0eb346ff1a",

  # googletest
  "ppi/third_party/gtest/gtest": Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + '145d05750b15324899473340c8dd5af50d125d33',

  # benchmark
  "ppi/third_party/benchmark/benchmark": "https://github.com/google/benchmark.git@4abdfbb802d1b514703223f5f852ce4a507d32d2",
}

hooks = [
  {
    "name": "gn_win",
    "pattern": ".",
    "action": [
      "download_from_google_storage",
      "--no_resume",
      "--platform=win32",
      "--no_auth",
      "--bucket", "chromium-gn",
      "-s", "ppi/buildtools/win/gn.exe.sha1",
    ],
  },
  {
    "name": "gn_mac",
    "pattern": ".",
    "action": [
      "download_from_google_storage",
      "--no_resume",
      "--platform=darwin",
      "--no_auth",
      "--bucket", "chromium-gn",
      "-s", "ppi/buildtools/mac/gn.sha1",
    ],
  },
  {
    "name": "gn_linux64",
    "pattern": ".",
    "action": [
      "download_from_google_storage",
      "--no_resume",
      "--platform=linux*",
      "--no_auth",
      "--bucket", "chromium-gn",
      "-s", "ppi/buildtools/linux64/gn.sha1",
    ],
  },
]
