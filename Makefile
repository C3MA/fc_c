CC = gcc
LD = ld
RM = rm -f
MKDIR= mkdir -p
COPY=cp

LIB_SOURCES=src/fc.c src/proto.c src/fcseq.c src/posix/hwal.c src/util.c src/fcclient.c src/fcserver.c  
LIB_NAME=libfc
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)
BUILD=build

CFLAGS=-c -Wall -fPIC
# Activate debugging with:
#CFLAGS += -DPRINT_DEBUG

LDFLAGS= -I $(BUILD)/include -L $(BUILD) -lfc -lm
LDFLAGS_LIBRARY= -I $(BUILD)/include -lm

all: $(BUILD)/client $(BUILD)/parsefile $(BUILD)/server

mksystem:
	$(MKDIR) $(BUILD)
	
$(BUILD)/client: libfc
	$(CC) -o $@ -I $(BUILD)/include example/client.c $(BUILD)/libfc.so

$(BUILD)/parsefile: libfc
	$(CC) -o $@ -I $(BUILD)/include example/parsefile.c  $(BUILD)/libfc.so

$(BUILD)/server: libfc
	$(CC) -o $@ -I $(BUILD)/include example/serverMain.c $(BUILD)/libfc.so


$(LIB_NAME): $(LIB_OBJECTS) mksystem
	$(MKDIR) $(BUILD)/include
	$(COPY) src/*.h $(BUILD)/include/
	$(CC) $(LDFLAGS_LIBRARY) -shared -o $(BUILD)/$@.so $(LIB_OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) src/*.o
	$(RM) src/posix/*.o
	$(RM) -r $(BUILD)
	$(RM) client parsefile cam

$(BUILD)/cam: $(LIB_NAME)
	gcc -o $@ example/cam.c $(BUILD)/$(LIB_NAME).so `pkg-config --cflags opencv` `pkg-config --libs opencv`  $(LDFLAGS)

$(BUILD)/ncserver: $(LIB_NAME)
	$(CC) -o $@ -I $(BUILD)/include -I /usr/include example/ncserver.c $(BUILD)/libfc.so -lncurses
	
docu:
	cd doc ; doxygen fc_c_api ; cd ..
