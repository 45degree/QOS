task("copy disk")
    on_run(function()
        os.cp("$(projectdir)/80m.img", "$(projectdir)/81m.img")
    end)
task_end()
