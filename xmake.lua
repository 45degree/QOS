set_config("cc", "gcc")
set_config("arch", "i386")

target("writebyte")
    set_kind("binary")
    add_files("$(projectdir)/tools/writeBytes.c")
target_end()

target("QOS.img")
    add_deps("writebyte")
    add_deps("kernel")
    add_deps("bootsector")
    add_deps("Loader")
    set_filename("QOS.img")
    before_build(function(target)
        if(not os.exists("/mnt/FlooyDisk")) then
            os.run("sudo mkdir /mnt/FlooyDisk")
        end
    end)
    on_build(function(target)
        import("core.project.project")
        local writebyte_target = project.target("writebyte"):targetfile()
        local bootsector_target = project.target("bootsector"):targetfile()
        local loader_target = project.target("Loader"):targetfile()
        local kernel_target = path.join(project.target("kernel"):targetdir(), "kernel.bin")
        os.run("dd if=/dev/zero of="..target:targetfile().." bs=1 count=1474560")
        os.run(writebyte_target.." -i "..bootsector_target.." -o "..target:targetfile().." -s 0")
        os.run("sudo mount "..target:targetfile().." /mnt/FlooyDisk")
        os.run("sudo cp "..kernel_target.." /mnt/FlooyDisk")
        os.run("sudo cp "..loader_target.." /mnt/FlooyDisk")
        os.run("sudo umount /mnt/FlooyDisk")
    end)

    after_build(function()
        import("core.project.task")
        task.run("copy disk")
    end)

    on_run(function(target)
        os.run("qemu-system-i386 -hda ./81m.img -S -gdb tcp::1234 -boot a -fda "..target:targetfile().." -serial stdio -m 2048")
        -- os.run("qemu-system-i386 -hda ./81m.img -boot a -fda "..target:targetfile().." -serial stdio")
    end)
target_end()

add_subdirs("init", "kernel", "script")
