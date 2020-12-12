add_requires("gtest")

target("test")
    set_kind("binary")
    add_defines("CONFIG_32")
    add_files("$(projectdir)/test/**.cc")
    add_includedirs("$(projectdir)/include/")
    add_includedirs("$(projectdir)/test/")
    add_packages("gtest")
