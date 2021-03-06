# -*- mode: makefile-gmake -*-
.SECONDEXPANSION:
.DELETE_ON_ERROR:

ALL: all

-include ./projdir.mk
PROJ_CONF_DIR = $(PROJ_DIR)/lib/$(PROJ_NAME)/conf
include $(PROJ_CONF_DIR)/projvariables
include $(PROJ_CONF_DIR)/projrules

# add the appropriate header-file inclusions and linker dependencies for each library we
# plan to build against
$(foreach dep,$(PROJ_DEPENDENCIES),$(eval $(call proj_add_dependency,$(dep))))

SRC_STRUCTURE := $(shell find $(PROJ_SRC_DIR) -type d)
SRC_TREE := $(addsuffix /*,$(SRC_STRUCTURE))
SRC_LEAVES := $(wildcard $(SRC_TREE))
SRC_ALL = $(filter %.cpp,$(SRC_LEAVES))

INCLUDE_TREE :=	$(shell find $(PROJ_INCLUDE_DIR) -name "*.hpp")
INCLUDES_ALL := $(filter %.hpp,$(INCLUDE_TREE))

OBJ  = $(subst $(PROJ_SRC_DIR),$(PROJ_OBJ_DIR),$(SRC_ALL:%.cpp=%.cpp.o))
DEPS = $(OBJ:%.o=%.d)

LIBS = $(PROJ_LIB_DIR)/$(PROJ_LONG_LIB_NAME)

.PHONY: all info clean all-clean libs all-local TAGS

help:
	@printf "Usage: make [MAKE_OPTIONS] [target] (see 'make --help' for MAKE_OPTIONS)\n"
	@printf ""
	@awk '								\
	{								\
	  if ($$0 ~ /^.PHONY: [a-zA-Z\-\0-9]+$$/) {			\
	    helpCommand = substr($$0, index($$0, ":") + 2);		\
	    if (helpMessage) {						\
	      printf "\033[36m%-20s\033[0m %s\n", helpCommand, helpMessage; \
	      helpMessage = "";						\
	    }								\
	  } else if ($$0 ~ /^[a-zA-Z\-\0-9.]+:/) {			\
	    helpCommand = substr($$0, 0, index($$0, ":"));		\
	    if (helpMessage) {						\
	      printf "\033[36m%-20s\033[0m %s\n", helpCommand, helpMessage; \
	      helpMessage = "";						\
	    }								\
	  } else if ($$0 ~ /^##/) {					\
	    if (helpMessage) {						\
	      helpMessage = helpMessage"\n                     "substr($$0, 3); \
	    } else {							\
	      helpMessage = substr($$0, 3);				\
	    }								\
	  } else {							\
	    if (helpMessage) {						\
	      print "\n                     "helpMessage"\n";		\
	    }								\
	    helpMessage = "";						\
	  }								\
	}'								\
	$(MAKEFILE_LIST)

## -- commonly used --

## delete all build-related files (commonly called dist-clean)
all-clean: clean
	@$(RM) -rf $(PROJ_BUILD_DIR)
	@$(RM) -f $(PROJ_DIR)/make.log

## delete all build files from the current arch directory
clean:
	@$(RM) -rf $(PROJ_OBJ_DIR) $(PROJ_LIB_DIR)

## build the library
all:
	@mkdir -p $(PROJ_ARCH)
	@>$(PROJ_ARCH)/make.log
	@ln -sf $(PROJ_ARCH)/make.log make.log
	+@$(OMAKE_SELF_PRINTDIR) PROJ_DIR=$(PROJ_DIR) PROJ_ARCH=$(PROJ_ARCH) all-local 2>&1 | tee -a $(PROJ_ARCH)/make.log

info:
	-@echo "=========================================="
	-@echo Starting make run on `hostname` at `date +'%a, %d %b %Y %H:%M:%S %z'`
	-@echo Machine characteristics: `uname -a`
	-@echo "-----------------------------------------"
	-@echo "Using PROJ directory: ${PROJ_DIR}"
	-@echo "Using PROJ arch:      ${PROJ_ARCH}"
	-@echo "Using PROJ version:   $(PROJ_VERSION)"
	-@echo "------------------------------------------"
	-@echo "Using CXX: $(shell which $(CXX))"
	-@echo "Using CXXFLAGS: $(PROJ_CXXFLAGS) $(CXXFLAGS)"
	-@echo "Using CXX Version: `$(CXX) --version`"
	-@echo "------------------------------------------"
	-@echo "Using MAKE: $(MAKE)"
	-@echo "Using MAKEFLAGS: -j$(MAKE_NP) -l$(MAKE_LOAD) $(MAKEFLAGS)"
	-@echo "=========================================="

all-local: info $(LIBS)

$(LIBS): $(OBJ) | $$(@D)/.DIR
	$(PROJ_LINK_CXX_SINGLE) -o $@ $^ $(PROJ_LIBS)
	@ln -sf $@ $(PROJ_LIB_DIR)/$(PROJ_LIB_NAME)

$(PROJ_OBJ_DIR)/%.cpp.o: $(PROJ_SRC_DIR)/%.cpp | $$(@D)/.DIR
	$(PROJ_COMPILE_CXX_SINGLE) -c $< -o $@

.PRECIOUS: %/.DIR

%/.DIR:
	-@mkdir -p $(@D)
	-@touch $@

TAGS: $(SRC_ALL)
	@$(RM) -f ./TAGS
	@etags $(SRC_ALL)

-include $(DEPS)
