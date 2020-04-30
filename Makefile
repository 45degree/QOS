# 编译器
export CC := clang
export ASM := nasm
export ASMFLAG := -f elf32
export CFLAG := -m32 -c -nostdinc -fno-builtin
export LDFLAG := -Ttext 0x30400 -m elf_i386

# 当前路径
export rootDir := $(shell pwd)

# 输出文件夹
export OutputDir := $(rootDir)/output/
export OutputTarget := $(OutputDir)/system.img
export imgMountPose := /mnt/FlooyDisk

# 用于向磁盘写数据的工具
writeBytes = $(OutputDir)/tools/writeBytes

# core文件夹下需要编译出的目标文件以及编译时需要包含的头文件
coreSrc := $(rootDir)/core/
coreOutputDir := $(OutputDir)
includePath := $(rootDir)/include

coreC := $(patsubst %.c, %.o, $(notdir $(wildcard $(coreSrc)/*.c)))
coreCTarget := $(addprefix $(coreOutputDir), $(coreC))

coreAsm := $(patsubst %.asm, %.o, $(notdir $(wildcard $(coreSrc)/*.asm)))
coreAsmTarget := $(addprefix $(coreOutputDir), $(coreAsm))

kernelFile := $(OutputDir)/kernel.bin

# lib文件夹下需要编译出的目标文件
libSrc := $(rootDir)/lib/
lib := $(patsubst %.asm, %.o, $(notdir $(wildcard $(libSrc)/*.asm)))
libOutputDir := $(OutputDir)
libTarget := $(addprefix $(libOutputDir), $(lib))

#init文件夹下需要编译出的目标文件
initSrc := $(rootDir)/init/
initOutputDir := $(OutputDir)
bootsector := $(initOutputDir)/bootsector.bin
Loader := $(initOutputDir)/Loader.bin
initTarget := $(bootsector) $(Loader)

#tool文件夹下需要编译出来的工具, 以及目标文件位置
tools := writeBytes
toolsSrc := $(rootDir)/tools/
toolsOutputDir := $(OutputDir)/tools/
toolsTarget := $(addprefix $(toolsOutputDir), $(tools))

#test
testSrc := $(rootDir)/test/
tests := $(wildcard $(coreSrc)/*.c) $(wildcard $(testSrc)/*.c)
testOutputDir := $(OutputDir)/test/
testTarget := $(addprefix $(testOutputDir), $(tests))

# 生成最终文件(软盘镜像), 并所有删除中间生成文件
all: buildInit buildTools buildKernel
	dd if=/dev/zero of=$(OutputTarget) bs=1 count=1474560
	$(writeBytes) -i $(bootsector) -o $(OutputTarget) -s 0
	sudo mount $(OutputTarget) $(imgMountPose)
	sudo cp $(kernelFile) $(imgMountPose)
	sudo cp $(Loader) $(imgMountPose)
	sudo umount $(imgMountPose)

# 编译init文件夹下文件
buildInit:$(initTarget)
$(initTarget):$(OutputDir)%.bin: $(initSrc)%.asm
	$(ASM) $< -o $@ -I $(initSrc)

# 生成工具文件
buildTools:$(toolsTarget)
$(toolsTarget):$(toolsOutputDir)%: $(toolsSrc)%.c
	$(CC) $< -o $@ -I $(toolsSrc)

# 编译kernel
buildKernel: .buildCore
	ld $(LDFLAG) $(coreAsmTarget) $(coreCTarget) $(libTarget) -o $(kernelFile)
	rm $(OutputDir)/*.o

# 编译core文件夹下文件
.buildCore: .buildCoreC .buildAsmCore .buildLib

# 编译core文件夹下c文件
.buildCoreC: $(coreCTarget)
$(coreCTarget): $(coreOutputDir)%.o: $(coreSrc)%.c
	$(CC) $(CFLAG) $< -o $@ -I $(includePath)

# 编译core文件夹下汇编文件
.buildAsmCore: $(coreAsmTarget)
$(coreAsmTarget): $(coreOutputDir)%.o: $(coreSrc)%.asm
	$(ASM) $(ASMFLAG) $< -o $@ -I $(includePath)

# 编译lib文件夹下文件
.buildLib:$(libTarget)
$(libTarget): $(libOutputDir)%.o: $(libSrc)%.asm
	nasm $(ASMFLAG) $< -o $@ -I $(includePath)

#运行
run:
	qemu-system-i386 -boot a -fda $(OutputTarget) -serial stdio -gdb tcp::1234

clean:
	rm $(OutputDir)/*.o $(OutputDir)/*.bin

# 调试
debug: $(OutputTarget)
	qemu-system-i386 -boot a -fda $(OutputTarget) -serial stdio -gdb tcp::1234 -S

bochs:
	bochs -f ./config/debug/bochsrc -q

Test:
	gcc $(tests) -o $(OutputDir)/test -m32 -I $(includePath)
	$(OutputDir)/test
