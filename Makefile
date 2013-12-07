CC = gcc
LD = ld
RM = rm -f
MKDIR= mkdir -p
COPY=cp

LIB_SOURCES=src/fc.c src/fcclient.c src/fcserver.c src/proto.c src/fcseq.c src/posix/hwal.c src/util.c
LIB_NAME=libfc
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)
BUILD=build

CFLAGS=-c -Wall
# Activate debugging with:
CFLAGS += -DPRINT_DEBUG
LDFLAGS= -I $(BUILD)/include -lm -L./$(BUILD) -lfc
LDFLAGS_LIBRARY= -I$(BUILD)/include -lm

all: client parsefile

mksystem:
	$(MKDIR) $(BUILD)
	
client: libfc
	$(CC) -o $@ example/client.c $(BUILD)/$(LIB_NAME).so $(LDFLAGS)

parsefile: libfc
	$(CC) -o $@ example/parsefile.c $(BUILD)/$(LIB_NAME).so $(LDFLAGS) 

server: libfc
	$(CC) -o $@ example/serverMain.c $(BUILD)/$(LIB_NAME).so $(LDFLAGS) 

$(LIB_NAME): $(LIB_OBJECTS) mksystem
	$(CC) $(LIB_OBJECTS) $(LDFLAGS_LIBRARY) -shared -o $(BUILD)/$@.so
	$(MKDIR) $(BUILD)/include
	$(COPY) src/*.h $(BUILD)/include/

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) src/*.o
	$(RM) src/posix/*.o
	$(RM) -r $(BUILD)
	$(RM) client parsefile cam

cam: $(LIB_NAME)
	gcc -o $@ example/cam.c $(BUILD)/$(LIB_NAME).so `pkg-config --cflags opencv` `pkg-config --libs opencv`  $(LDFLAGS)

docu:
	cd doc ; doxygen fc_c_api ; cd ..
