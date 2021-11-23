

# Location of the OpenCL library, where libOpenCL.so resides
CL_LIB=/usr/local/cuda/lib64
# Location of the OpenCL CL directory where cl.h and cl.hpp reside
CL_INCLUDE=/usr/local/cuda/include

# C++ compiler and flags
CXX=g++

ifeq ($(OS),Windows_NT)
	CXXFLAGS=-g -O3 -fPIC -I$(CL_INCLUDE) -std=c++11
	LFLAGS=-g -L$(CL_LIB) -lstdc++         
else
	uname_s := $(shell uname -s)
	ifeq ($(uname_s),Linux)
		CXXFLAGS=-g -O3 -fPIC -I$(CL_INCLUDE) -std=c++11
		LFLAGS=-g -L$(CL_LIB) -lstdc++          
	endif
	ifeq ($(uname_s),Darwin)
		CXXFLAGS=-g -O3 -fPIC -std=c++11
		LFLAGS=-g -lstdc++ -framework OpenCL        
	endif
endif

# Matrix multiplication
all:	convolution \
		template	

convolution:	convolution.o
	$(CXX) $(LFLAGS) -o $@ $< -lOpenCL

template:	template.o
	$(CXX) $(LFLAGS) -o $@ $< -lOpenCL

%.o:	%.cpp cl_helper.hpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -rf *.o *.mod *.bin \
    mat_mult \
    copy_rect_region \
    mat_mult_create_binary \
    mat_mult_use_binary \
    mat_mult_transpose \
    mat_mult_transpose_vector \
    template
