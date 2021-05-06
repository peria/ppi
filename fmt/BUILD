cc_library(
  name = "fmt",
  srcs = [],
  hdrs = [
    "fmt.h",
  ],
  visibility = [ "//visibility:public" ],
  deps = [ "//base" ],
)

cc_library(
  name = "dft",
  srcs = [
    "dft.cc",
  ],
  hdrs = [
    "dft.h",
  ],
  visibility = [ "//visibility:public" ],
  deps = [
    ":fmt",
    "//base",
    "//number:complex_number",
  ],
)
