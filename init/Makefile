target := $(addprefix $(OutputDir), $(initTarget))

all: $(target)
$(target): $(OutputDir)%.bin: %.asm
	nasm -g $< -o $@