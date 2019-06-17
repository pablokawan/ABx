include common.make

# This may change
TARGETDIR = ../Lib/x64/Release
TARGET = $(TARGETDIR)/libless.a
SOURDEDIR = ../ThirdParty/libless/src
OBJDIR = obj/x64/Release/libless
# End changes

SRC_FILES = $(wildcard $(SOURDEDIR)/*.cpp $(SOURDEDIR)/*/*.cpp)

CXXFLAGS += $(DEFINES) $(INCLUDES)

OBJ_FILES := $(patsubst $(SOURDEDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC_FILES))
#$(info $(OBJ_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@$(MKDIR_P) $(@D)
	$(LINKCMD_LIB) $(OBJ_FILES)

$(OBJDIR)/%.o: $(SOURDEDIR)/%.cpp
	@$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(OBJ_FILES:.o=.d)

.PHONY: clean
clean:
	rm -f $(OBJ_FILES) $(TARGET)
