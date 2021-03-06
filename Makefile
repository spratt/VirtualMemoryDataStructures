##############################################################
#               CMake Project Wrapper Makefile               #
#															 #
# Taken from:												 #
#   https://code.google.com/archive/p/cpp-project-template/  #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf

.PHONY: all distclean plot

all: ./build/Makefile
	@ $(MAKE) -C build

./build/Makefile:
	@ (cd build >/dev/null 2>&1 && cmake ..)

distclean:
	@- (cd build >/dev/null 2>&1 && cmake .. >/dev/null 2>&1)
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build/Makefile
	@- $(RM) ./build/src
	@- $(RM) ./build/test
	@- $(RM) ./build/CMake*
	@- $(RM) ./build/cmake.*
	@- $(RM) ./build/*.cmake
	@- $(RM) ./build/*.txt


ifeq ($(findstring distclean,$(MAKECMDGOALS)),)

    $(MAKECMDGOALS): ./build/Makefile
	@ $(MAKE) -C build $(MAKECMDGOALS)

endif
