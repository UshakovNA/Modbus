
TARGET := test
# TARGET := smbxv3_app
CXX := g++
CXXFLAGS = -std=c++17 -Wall -g -O0
DEPDIR := deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
INCLUDES = 
LIBS =
OBJDIR = obj

SOURCES = main.cpp
SRC_DIRS = .

##---------------------------------------------------------------------
## APPLICATION
##---------------------------------------------------------------------
SOURCES += App.cpp
SRC_DIRS += App
##---------------------------------------------------------------------
## MODBUS
##---------------------------------------------------------------------
SOURCES += Modbus.cpp
SRC_DIRS += modbus 
##---------------------------------------------------------------------
## SERIAL PORT
##---------------------------------------------------------------------
SOURCES += SerialPort.cpp
SRC_DIRS += comms 
##---------------------------------------------------------------------
## myLogger
##---------------------------------------------------------------------
SOURCES += myLog.cpp
SRC_DIRS += myLog  

OBJECTS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

vpath %.cpp $(sort $(SRC_DIRS))
INCLUDES += $(addprefix -I, $(SRC_DIRS))

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@

all: $(TARGET)
	@echo "Build OK"

$(TARGET): $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CXX) $^ -o $@ $(LIBS)

include $(wildcard *.d) 

.PHONY: clean
clean: 
	rm $(wildcard deps/*.d) $(wildcard $(OBJDIR)/*.o) $(wildcard $(TARGET)) 2>/dev/null

## Other dependencies
-include $(shell mkdir deps 2>/dev/null) $(wildcard deps/*) 
-include $(shell mkdir $(OBJDIR) 2>/dev/null)
