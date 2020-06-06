rule("asm binaray")
    set_extensions(".asm")
    on_build(function(target)
        for _, file in ipairs(target:sourcefiles())
        do
            local output_file = target:targetdir().."/"..path.basename(file)..".bin"
            os.run("nasm -o"..output_file.." "..file.." -I $(projectdir)/init")
        end
    end)


target("bootsector")
    add_rules("asm binaray")
    set_filename("bootsector.bin")
    add_files("$(projectdir)/init/bootsector.asm")
target_end()

target("Loader")
    add_rules("asm binaray")
    set_filename("Loader.bin")
    add_files("$(projectdir)/init/Loader.asm")
target_end()