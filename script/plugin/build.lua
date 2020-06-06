task("system")
    set_category("plugin")
    on_run(function ()

        -- 显示hello xmake!
        print("hello xmake!")
    end)

    set_menu {
        -- usage
        usage = "xmake hello [options]",

        -- description
        description = "Hello xmake!",

        -- options
        options = {}
    } 