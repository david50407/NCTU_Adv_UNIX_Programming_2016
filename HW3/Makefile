CC:= gcc
CFLAGS:= -I./include
CXX:= g++
CXXFLAGS:= -I./include --std=c++14 -fPIC -fpermissive
LD:= g++
LDFLAGS:= $(CXXFLAGS)
OBJS:= main.o shell.o command.o util.o signal_handler.o process_manager.o
EXEC:= shell

.PHONY: all run clean

all: $(EXEC)

run: $(EXEC)
	-./$(EXEC)

clean:
	-rm $(OBJS) $(EXEC)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXEC): $(OBJS)
	$(LD) $(LDFLAGS) -o $(EXEC) $(OBJS)
