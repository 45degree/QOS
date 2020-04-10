target := $(addprefix $(toolsTargetDir), $(toolsTarget))

all: $(target)
$(target): $(toolsTargetDir)%: %.c
	$(CC) $< -o $@