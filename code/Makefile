

# Location of the OpenCL library, where libOpenCL.so resides
CL_LIB=/usr/local/cuda/lib64
# Location of the OpenCL CL directory where cl.h and cl.hpp reside
CL_INCLUDE=/usr/local/cuda/include

# C++ compiler and flags
CXX=g++

ifeq ($(OS),Windows_NT)
	CXXFLAGS=-g -O3 -fPIC -I$(CL_INCLUDE) -std=c++11
	LFLAGS=-g -L$(CL_LIB) -lstdc++ -lOpenCL         
else
	uname_s := $(shell uname -s)
	ifeq ($(uname_s),Linux)
		CXXFLAGS=-g -O3 -fPIC -I$(CL_INCLUDE) -std=c++11
		LFLAGS=-g -L$(CL_LIB) -lstdc++ -lOpenCL		
	endif
	ifeq ($(uname_s),Darwin)
		CXXFLAGS=-g -O3 -fPIC -std=c++11
		LFLAGS=-g -lstdc++ -framework OpenCL        
	endif
endif

# Matrix multiplication
all:    mat_mult \
	copy_rect_region \
	mat_mult_create_binary \
	mat_mult_use_binary \
	mat_mult_transpose \
	mat_mult_transpose_vector \
    template

mat_mult:	mat_mult.o
	$(CXX) -o $@ $< $(LFLAGS)

mat_mult_create_binary:	mat_mult_create_binary.o
	$(CXX) -o $@ $< $(LFLAGS)

mat_mult_use_binary:	mat_mult_use_binary.o
	$(CXX) -o $@ $< $(LFLAGS)

copy_rect_region:	copy_rect_region.o
	$(CXX) -o $@ $< $(LFLAGS)

mat_mult_transpose:	mat_mult_transpose.o
	$(CXX) -o $@ $< $(LFLAGS)

mat_mult_transpose_vector:	mat_mult_transpose_vector.o
	$(CXX) -o $@ $< $(LFLAGS)

template:	template.o
	$(CXX) -o $@ $< $(LFLAGS)

%.o:	%.cpp helper_functions.hpp
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
