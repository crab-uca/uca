CC=g++
CFLAGS=-Wall -I/opt/amdgpu-pro/include
LDFLAGS=-L/opt/amdgpu-pro/lib/x86_64-linux-gnu -lOpenCL

TARGET=hw
SRCS=HelloWorld.cpp
OBJS=$(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(TARGET) *.o
