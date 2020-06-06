add_cflags("-g", "-nostdinc", "-fno-builtin", {force = true})

target("kernel")
    set_kind("object")
    add_includedirs("$(projectdir)/include")

    add_files("$(projectdir)/kernel/**.c")

    after_build(function (target)
        -- 编译所有nasm汇编文件
        for _, input_filepath in ipairs(os.files("$(projectdir)/kernel/**.asm")) do
            os.cd("$(projectdir)")
            r_input_filepath = path.relative(input_filepath)
            output_filepath = path.join(target:objectdir(), r_input_filepath..".o")

            if not(os.isdir(path.directory(output_filepath))) then
                os.mkdir(path.directory(output_filepath))
            end

            os.run("nasm -f elf32 "..input_filepath.." -o "..output_filepath.." -I "..target:get("includedirs"))
        end

        -- 生成链接脚本
        local date = io.readfile("$(projectdir)/script/kernel.lds.template")
        date = string.format(date, path.join(target:objectdir(), "kernel/kernel.asm.o"))
        io.writefile(path.join(target:objectdir(), "kernel/kernel.lds"), date)
        local link_script = path.join(target:objectdir(),"kernel/kernel.lds")

        -- 查找链接源文件
        local link_src = ""
        for _, filepath in ipairs(os.files(path.join(target:objectdir(),"kernel/**.o"))) do
            link_src = link_src.." "..filepath 
        end

        -- 执行链接指令
        local link_output = path.join(target:targetdir(), "kernel.bin")
        local link_cmd = format("ld -m elf_i386 -T%s %s -o %s", link_script, link_src, link_output)
        os.run(link_cmd)
    end)
