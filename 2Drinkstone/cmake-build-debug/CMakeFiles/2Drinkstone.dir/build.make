# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.ap1ixGvmCJ

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.ap1ixGvmCJ/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/2Drinkstone.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/2Drinkstone.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/2Drinkstone.dir/flags.make

CMakeFiles/2Drinkstone.dir/main.c.o: CMakeFiles/2Drinkstone.dir/flags.make
CMakeFiles/2Drinkstone.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ap1ixGvmCJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/2Drinkstone.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/2Drinkstone.dir/main.c.o -c /tmp/tmp.ap1ixGvmCJ/main.c

CMakeFiles/2Drinkstone.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/2Drinkstone.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ap1ixGvmCJ/main.c > CMakeFiles/2Drinkstone.dir/main.c.i

CMakeFiles/2Drinkstone.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/2Drinkstone.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ap1ixGvmCJ/main.c -o CMakeFiles/2Drinkstone.dir/main.c.s

# Object files for target 2Drinkstone
2Drinkstone_OBJECTS = \
"CMakeFiles/2Drinkstone.dir/main.c.o"

# External object files for target 2Drinkstone
2Drinkstone_EXTERNAL_OBJECTS =

2Drinkstone: CMakeFiles/2Drinkstone.dir/main.c.o
2Drinkstone: CMakeFiles/2Drinkstone.dir/build.make
2Drinkstone: CMakeFiles/2Drinkstone.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.ap1ixGvmCJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable 2Drinkstone"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/2Drinkstone.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/2Drinkstone.dir/build: 2Drinkstone

.PHONY : CMakeFiles/2Drinkstone.dir/build

CMakeFiles/2Drinkstone.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/2Drinkstone.dir/cmake_clean.cmake
.PHONY : CMakeFiles/2Drinkstone.dir/clean

CMakeFiles/2Drinkstone.dir/depend:
	cd /tmp/tmp.ap1ixGvmCJ/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.ap1ixGvmCJ /tmp/tmp.ap1ixGvmCJ /tmp/tmp.ap1ixGvmCJ/cmake-build-debug /tmp/tmp.ap1ixGvmCJ/cmake-build-debug /tmp/tmp.ap1ixGvmCJ/cmake-build-debug/CMakeFiles/2Drinkstone.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/2Drinkstone.dir/depend

