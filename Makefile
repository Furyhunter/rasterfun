SRCS := $(wildcard src/*.cpp)
OBJS := $(addprefix obj/, $(notdir $(SRCS:.cpp=.o)))

PROJECTNAME := rasterfun

# dep graph
CFLAGS += -MMD
-include $(OBJECTS:.o=.d)

CFLAGS  += --std=c++11
LDFLAGS += $(shell sdl2-config --libs)
CFLAGS  += $(shell sdl2-config --cflags)

CFLAGS += -g
LDFLAGS += -g

all: $(PROJECTNAME)

$(PROJECTNAME): $(OBJS)
	g++ $^ $(LDFLAGS) -o $@

obj/%.o: src/%.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean:
	rm -rf obj/*