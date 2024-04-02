CXXFLAGS	:= -std=c++17 -Wall -Wextra -Werror -ggdb3
# CXXFLAGS	+= -O3 -momit-leaf-frame-pointer
CXXFLAGS	+= -I.
LDLIBS		:=
TARGETS		:= $(patsubst %.cpp, %, $(wildcard test*.cpp))

DEPEND_DIR	:= .depend

CLEAN_FILES	:= $(TARGETS)
CLEAN_FILES	+= $(TARGETS:%=%.o)
CLEAN_FILES	+= $(TARGETS:%=.%.d)

CLEAN_DIRS	+= $(DEPEND_DIR)

all: $(TARGETS)

$(TARGETS): %: %.o
	$(CXX) $(LDFLAGS) -o $@ $*.o $(LDLIBS)

%.o: %.cpp | $(DEPEND_DIR)
	$(CXX) $(CXXFLAGS) -MD -MF $(DEPEND_DIR)/$*.d -o $@ -c $*.cpp

$(DEPEND_DIR):
	mkdir $(DEPEND_DIR)

-include $(wildcard $(DEPEND_DIR)/*.d)

CLEAN_FILES	:= $(wildcard $(CLEAN_FILES))
CLEAN_DIRS	:= $(wildcard $(CLEAN_DIRS))
clean:
ifdef CLEAN_FILES
	$(RM) $(CLEAN_FILES)
endif
ifdef CLEAN_DIRS
	$(RM) -r $(CLEAN_DIRS)
endif

.NOTPARALLEL:
