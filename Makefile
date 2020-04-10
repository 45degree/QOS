# 编译器
export CC := clang
export ASM := nasm
export ASMFLAG := -g -f elf32
export CFLAG := -m32 -c -nostdinc -ggdb -fno-builtin
export LDFLAG := -Ttext 0x30400 -m elf_i386

# 当前路径
rootDir := $(shell pwd)

# 输出文件夹
export OutputDir := $(rootDir)/output/

# core文件夹下需要编译出的目标文件以及编译时需要包含的头文件
export coreCTarget := interrupt.o global.o show.o main.o
export coreAsmTarget := kernel.o
export includPath := $(rootDir)/include

# lib文件夹下需要编译出的目标文件
export libTarget := common.o displayText.o

#init文件夹下需要编译出的目标文件
export bootsector := bootsector.bin
export Loader := Loader.bin
export initTarget := $(bootsector) $(Loader)

#tool文件夹下需要编译出来的工具, 以及目标文件位置
export toolsTarget := writeBytes
export toolsTargetDir := $(rootDir)/output/tools/

#output文件夹下生成的目标和相关依赖
export kernelDep := $(coreAsmTarget) $(coreCTarget) $(libTarget)
export OutputTarget := system.img
export imgMountPose := /mnt/FlooyDisk

# 生成最终文件(软盘镜像), 并所有删除中间生成文件
all: buildOutput
	$(MAKE) clean

# 编译init文件夹下文件
buildInit:
	$(MAKE) -C ./init/

# 生成工具文件
buildTools:
	$(MAKE) -C ./tools/

# 编译core文件夹下文件
buildCore:
	$(MAKE) -C ./core/

# 编译lib文件夹下文件
buildLib:
	$(MAKE) -C ./lib/

# 在output文件夹下生成最终文件(软盘镜像), 不删除中间生成文件
buildOutput: buildInit buildTools buildCore buildLib
	$(MAKE) -C $(OutputDir)

#运行
run:
	qemu-system-i386 -boot a -fda $(OutputDir)/$(OutputTarget) -serial stdio -gdb tcp::1234

# 调试
debug: buildOutput
	qemu-system-i386 -boot a -fda $(OutputDir)/$(OutputTarget) -serial stdio -gdb tcp::1234 -S

# 删除最终生成的软盘镜像
cleanTarget:
	-rm $(OutputDir)/$(OutputTarget)

# 删除工具文件
cleanTools:
	-rm $(toolsTargetDir)/*

# 删除除最终生成文件外的所有文件
clean:
	$(MAKE) clean -C $(OutputDir)

# 删除除tools文件夹下所有中间文件
cleanExceptTool:
	$(MAKE) cleanExceptTool -C $(OutputDir)

# 删除output下所有生成的文件
cleanAll:
	$(MAKE) cleanAll -C $(OutputDir)
