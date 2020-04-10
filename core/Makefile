CTaregt := $(addprefix $(OutputDir), $(coreCTarget))
AsmTarget := $(addprefix $(OutputDir), $(coreAsmTarget))

all: $(CTaregt) $(AsmTarget)

# 编译lib为elf32格式的文件
$(CTaregt): $(OutputDir)%.o: %.c
	$(CC) $(CFLAG) $< -o $@ -I $(includPath)

$(AsmTarget): $(OutputDir)%.o: %.asm
	nasm $(ASMFLAG) -o $@ $<