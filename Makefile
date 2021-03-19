LIBS := -g -ludev
LIBS += $(shell PKG_CONFIG_PATH=$(PKG_CFG_PATH) pkg-config --libs botan-2)

.PHONY: all clean

MAIN_DIR := .
APP2 := license
LICENSE_SRC := $(wildcard src/$(APP2).cpp)

APP := get_data
#APP := get_license
#APP := app
MAIN_SRCS := $(APP).cpp

CFLAGS += -I$(MAIN_DIR)/include
CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CFG_PATH) pkg-config --cflags botan-2) -std=c++11

all: $(APP)

$(APP): $(LICENSE_SRC :.cpp=.o) $(MAIN_SRCS:.cpp=.o)
	g++  -o $@ $^ $(LIBS)

$(MAIN_SRCS:.cpp=.o): %.o : $(MAIN_DIR)/src/%.cpp
	g++ -o $@ -c $< $(CFLAGS)

$(LICENSE_SRC :.cpp=.o): %.o : $(MAIN_DIR)/src/%.cpp
	g++ -o $@ -c $< $(CFLAGS)

clean:
	rm -fr *.o
