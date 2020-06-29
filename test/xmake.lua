target("Test")
    set_kind("binary")
    add_rules("mode.debug")
    set_symbols("debug")
    set_strip("all")
    add_files("$(projectdir)/kernel/**.c")
    add_files("$(projectdir)/test/**.c")
    add_includedirs("$(projectdir)/include")
    add_includedirs("$(projectdir)/test/")
