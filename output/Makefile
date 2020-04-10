kernelFile = kernel.bin

# 用于向磁盘写数据的工具
writeBytes = tools/writeBytes

$(OutputTarget): $(kernelFile) $(initTarget) $(writeBytes)
	dd if=/dev/zero of=$(OutputTarget) bs=1 count=1474560
	$(writeBytes) -i $(bootsector) -o $(OutputTarget) -s 0
	sudo mount $(OutputTarget) $(imgMountPose)
	sudo cp $(kernelFile) $(imgMountPose)
	sudo cp $(Loader) $(imgMountPose)
	sudo umount $(imgMountPose)

$(kernelFile): $(kernelDep)
	ld $(LDFLAG) -o $(kernelFile) $(kernelDep)

# 删除除tools文件夹下所有中间文件
cleanExceptTool:
	-rm $(kernelDep) $(kernelFile) $(initTarget)

# 删除除最终生成文件外的所有文件
clean:
	-rm $(kernelDep) $(kernelFile) $(initTarget) tools/$(toolsTarget)

cleanAll:
	-rm $(kernelDep) $(kernelFile) $(initTarget) tools/$(toolsTarget) $(OutputTarget)