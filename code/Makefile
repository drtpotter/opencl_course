

# Location of the OpenCL library, where libOpenCL.so resides
CL_LIB=/usr/local/cuda/lib64
# Location of the OpenCL CL directory where cl.h and cl.hpp reside
CL_INCLUDE=/usr/local/cuda/include

# C++ compiler and flags
CXX=g++
CXXFLAGS=-g -O0 -fPIC -I$(CL_INCLUDE) -std=c++11

# Fortran compiler and flags
FC=gfortran
FFLAGS=-g -O0 -fPIC 

# Link flags
LFLAGS=-g -L$(CL_LIB) -lOpenCL -lstdc++  

# Matrix multiplication
all:    mat_mult copy_rect_region mat_mult_create_binary mat_mult_use_binary mat_mult_transpose

mat_mult:	mat_mult.o
	$(CXX) $(LFLAGS) -o $@ $<

mat_mult_create_binary:	mat_mult_create_binary.o
	$(CXX) $(LFLAGS) -o $@ $<

mat_mult_use_binary:	mat_mult_use_binary.o
	$(CXX) $(LFLAGS) -o $@ $<

copy_rect_region:	copy_rect_region.o
	$(CXX) $(LFLAGS) -o $@ $<

mat_mult_transpose:	mat_mult_transpose.o
	$(CXX) $(LFLAGS) -o $@ $<

%.o:	%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -rf *.o *.mod *.bin mat_mult copy_rect_region mat_mult_create_binary mat_mult_use_binary mat_mult_transpose
