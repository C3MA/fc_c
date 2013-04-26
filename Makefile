CC = gcc
LD = ld
RM = rm -f
MKDIR= mkdir -p
COPY=cp

LIB_SOURCES=src/fc.c src/fcclient.c src/proto.c
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)
BUILD=build

CFLAGS=-c -Wall
LDFLAGS= -I $(BUILD)/include

all: client

mksystem:
	$(MKDIR) $(BUILD)
	
client: libfcclient mksystem
	$(CC) -o client example/client.c $(BUILD)/libfcclient.so $(LDFLAGS)

libfcclient: $(LIB_OBJECTS) mksystem
	$(CC) $(LIB_OBJECTS) $(LDFLAGS) -shared -o $(BUILD)/$@.so
	$(MKDIR) $(BUILD)/include
	$(COPY) src/*.h $(BUILD)/include/

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) src/*.o
	$(RM) -r $(BUILD)
