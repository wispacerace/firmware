add_library(Boost::vendored INTERFACE IMPORTED)
target_include_directories(Boost::vendored INTERFACE "${VendoredBoost_ROOT}/include")