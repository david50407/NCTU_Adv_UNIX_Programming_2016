CC:= gcc
CFLAGS:=
CXX:= g++
CXXFLAGS:= --std=c++14 -fPIC -fpermissive
LD:= g++
LDFLAGS:= $(CXXFLAGS)
OBJS:= webserver.o
EXEC:= webserver

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

