# Add .d to Make's recognized suffixes.
SUFFIXES += .d
SUFFIXES += .template

C = gcc
CU = nvcc

CFLAGS = -O3 -Wall
CUFLAGS = -O3 -arch sm_13
EXEC = sw\#

#Find all the C++ files in the src/ directory
SOURCES:=$(shell find src/ -regex .*.cu?)
#These are the dependency files, which make will clean up after it creates them
DEPFILES:=$(patsubst %.c,%.d,$(SOURCES))

cuda: $(patsubst src/%,obj/%, $(patsubst %.c,%.o, $(patsubst %.cu, %.o, $(SOURCES))))
	$(CU) $(CUDAFLAGS) -o $(EXEC) $^

#This is the rule for creating the dependency files
src/%.d: src/%.c
	$(C) $(CFLAGS) -MM -MT '$(patsubst src/%,obj/%,$(patsubst %.c,%.o,$<))' $< > $@

src/%.d: src/%.cu
	$(CU) $(CUFLAGS) -M '$(patsubst src/%,obj/%,$(patsubst %.cu,%.o,$<))' $< > $@

obj/main.o: src/main.c src/main.d 
	$(C) $(CFLAGS) -o $@ -c $<
	rm src/main.d 
	
#This rule does the compilation
obj/%.o: src/%.c src/%.d src/%.h
	$(C) $(CFLAGS) -o $@ -c $<

#This rule does the compilation
obj/%.o: src/%.cu src/%.d src/%.h
	$(CU) $(CUFLAGS) -o $@ -c $<

clean :
	rm obj/*.o $(EXEC)


