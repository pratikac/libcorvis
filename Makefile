BUILD_SYSTEM:=$(shell uname -s)
BUILD_SYSTEM:=$(strip $(BUILD_SYSTEM))

# Figure out where to build the software.
#   Use BUILD_PREFIX if it was passed in.
#   If not, search up to four parent directories for a 'build' directory.
#   Otherwise, use ./build.
ifeq "$(BUILD_PREFIX)" ""
BUILD_PREFIX:=$(shell for pfx in ./ .. ../.. ../../.. ../../../..; do d=`pwd`/$$pfx/build;\
               if [ -d $$d ]; then echo $$d; exit 0; fi; done; echo `pwd`/build)
endif
# create the build directory if needed, and normalize its path name
BUILD_PREFIX:=$(shell mkdir -p $(BUILD_PREFIX) && cd $(BUILD_PREFIX) && echo `pwd`)

# Default to a release build.  If you want to enable debugging flags, run
# "make BUILD_TYPE=Debug"
ifeq "$(BUILD_TYPE)" ""
BUILD_TYPE="Release"
endif

# force cmake configure if the BUILD_PREFIX doesn't match the cmake cache
CMAKE_INSTALL_PREFIX=$(shell cd pod-build 2> /dev/null && cmake -L 2> /dev/null | grep CMAKE_INSTALL_PREFIX | cut -d "=" -f2 | tr -d '[:space:]')
ifneq "$(BUILD_PREFIX)" "$(CMAKE_INSTALL_PREFIX)"
	OUT:=$(shell echo "\nBUILD_PREFIX $(BUILD_PREFIX) does not match CMAKE cache $(CMAKE_INSTALL_PREFIX).  Forcing configure\n\n"; touch CMakeLists.txt)
endif


all: pod-build/Makefile
	$(MAKE) -C pod-build install

pod-build/Makefile:
	$(MAKE) configure

.PHONY: configure
configure:
	@echo "BUILD_PREFIX: $(BUILD_PREFIX)"

	@mkdir -p pod-build

	# run CMake to generate and configure the build scripts
	@cd pod-build && cmake $(CMAKE_FLAGS) -DCMAKE_INSTALL_PREFIX="$(BUILD_PREFIX)" \
		   -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

test:
	$(MAKE) -C pod-build all-tests

.PHONY: doc
doc:
	doxygen Doxyfile

clean:
	-if [ -e pod-build/install_manifest.txt ]; then rm -f `cat pod-build/install_manifest.txt`; fi
	-if [ -d pod-build ]; then $(MAKE) -C pod-build clean; rm -rf pod-build; fi

# other (custom) targets are passed through to the cmake-generated Makefile
%::
	$(MAKE) -C pod-build $@

# Default to a less-verbose build.  If you want all the gory compiler output,
# run "make VERBOSE=1"
$(VERBOSE).SILENT:
