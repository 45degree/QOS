target := $(addprefix $(OutputDir), $(libTarget))

all: $(target)
$(target): $(OutputDir)%.o: %.asm
	nasm $(ASMFLAG) $< -o $@