OPENCLC=/System/Library/Frameworks/OpenCL.framework/Libraries/openclc
BUILD_DIR=./build
EXECUTABLE=main
.SUFFIXES:
KERNEL_ARCH=i386 x86_64 gpu_32 gpu_64
BITCODES=$(patsubst %, mykernel.cl.%.bc, $(KERNEL_ARCH))

$(EXECUTABLE): $(BUILD_DIR)/mykernel.cl.o $(BUILD_DIR)/main.o $(BITCODES)
	clang -framework OpenCL -o $@ $(BUILD_DIR)/mykernel.cl.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/mykernel.cl.o: mykernel.cl.c
	mkdir -p $(BUILD_DIR)
	clang -c -Os -Wall -arch x86_64 -o $@ -c mykernel.cl.c

$(BUILD_DIR)/main.o: main.c mykernel.cl.h
	mkdir -p $(BUILD_DIR)
	clang -c -Os -Wall -arch x86_64 -o $@ -c $<

mykernel.cl.c mykernel.cl.h: mykernel.cl
	$(OPENCLC) -x cl -cl-std=CL1.1 -cl-auto-vectorize-enable -emit-gcl $<

mykernel.cl.%.bc: mykernel.cl
	$(OPENCLC) -x cl -cl-std=CL1.1 -Os -arch $* -emit-llvm -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) mykernel.cl.h mykernel.cl.c $(EXECUTABLE) *.bc
