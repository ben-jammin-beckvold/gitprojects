# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/exec/Documents/vl53l0x-linux

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/exec/Documents/vl53l0x-linux/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/singleMinimal.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/singleMinimal.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/singleMinimal.dir/flags.make

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o: examples/CMakeFiles/singleMinimal.dir/flags.make
examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o: ../examples/singleMinimal.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/exec/Documents/vl53l0x-linux/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o -c /home/exec/Documents/vl53l0x-linux/examples/singleMinimal.cpp

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/singleMinimal.dir/singleMinimal.cpp.i"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/exec/Documents/vl53l0x-linux/examples/singleMinimal.cpp > CMakeFiles/singleMinimal.dir/singleMinimal.cpp.i

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/singleMinimal.dir/singleMinimal.cpp.s"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/exec/Documents/vl53l0x-linux/examples/singleMinimal.cpp -o CMakeFiles/singleMinimal.dir/singleMinimal.cpp.s

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.requires:

.PHONY : examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.requires

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.provides: examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/singleMinimal.dir/build.make examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.provides.build
.PHONY : examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.provides

examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.provides.build: examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o


examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o: examples/CMakeFiles/singleMinimal.dir/flags.make
examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o: ../VL53L0X.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/exec/Documents/vl53l0x-linux/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o -c /home/exec/Documents/vl53l0x-linux/VL53L0X.cpp

examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.i"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/exec/Documents/vl53l0x-linux/VL53L0X.cpp > CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.i

examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.s"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/exec/Documents/vl53l0x-linux/VL53L0X.cpp -o CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.s

examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.requires:

.PHONY : examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.requires

examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.provides: examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/singleMinimal.dir/build.make examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.provides.build
.PHONY : examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.provides

examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.provides.build: examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o


examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o: examples/CMakeFiles/singleMinimal.dir/flags.make
examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o: ../I2Cdev.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/exec/Documents/vl53l0x-linux/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o -c /home/exec/Documents/vl53l0x-linux/I2Cdev.cpp

examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.i"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/exec/Documents/vl53l0x-linux/I2Cdev.cpp > CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.i

examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.s"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/exec/Documents/vl53l0x-linux/I2Cdev.cpp -o CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.s

examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.requires:

.PHONY : examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.requires

examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.provides: examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/singleMinimal.dir/build.make examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.provides.build
.PHONY : examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.provides

examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.provides.build: examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o


# Object files for target singleMinimal
singleMinimal_OBJECTS = \
"CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o" \
"CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o" \
"CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o"

# External object files for target singleMinimal
singleMinimal_EXTERNAL_OBJECTS =

examples/singleMinimal: examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o
examples/singleMinimal: examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o
examples/singleMinimal: examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o
examples/singleMinimal: examples/CMakeFiles/singleMinimal.dir/build.make
examples/singleMinimal: examples/CMakeFiles/singleMinimal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/exec/Documents/vl53l0x-linux/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable singleMinimal"
	cd /home/exec/Documents/vl53l0x-linux/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/singleMinimal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/singleMinimal.dir/build: examples/singleMinimal

.PHONY : examples/CMakeFiles/singleMinimal.dir/build

examples/CMakeFiles/singleMinimal.dir/requires: examples/CMakeFiles/singleMinimal.dir/singleMinimal.cpp.o.requires
examples/CMakeFiles/singleMinimal.dir/requires: examples/CMakeFiles/singleMinimal.dir/__/VL53L0X.cpp.o.requires
examples/CMakeFiles/singleMinimal.dir/requires: examples/CMakeFiles/singleMinimal.dir/__/I2Cdev.cpp.o.requires

.PHONY : examples/CMakeFiles/singleMinimal.dir/requires

examples/CMakeFiles/singleMinimal.dir/clean:
	cd /home/exec/Documents/vl53l0x-linux/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/singleMinimal.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/singleMinimal.dir/clean

examples/CMakeFiles/singleMinimal.dir/depend:
	cd /home/exec/Documents/vl53l0x-linux/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/exec/Documents/vl53l0x-linux /home/exec/Documents/vl53l0x-linux/examples /home/exec/Documents/vl53l0x-linux/build /home/exec/Documents/vl53l0x-linux/build/examples /home/exec/Documents/vl53l0x-linux/build/examples/CMakeFiles/singleMinimal.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/singleMinimal.dir/depend

