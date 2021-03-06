
# require destination of this library in $(APP_LIBS)

FCPWD=$(APP_LIBS)/fc/src

# Append  all the source files.
LIBSRC += ${FCPWD}/fc.c \
${FCPWD}/proto.c \
${FCPWD}/fcseq.c \
${FCPWD}/util.c \
${FCPWD}/fcserver.c \
${FCPWD}/chibios/hwal.c \
${FCPWD}/chibios/hwal_net.c \

# Add required include directories
LIBINC += ${FCPWD} \
	 $(FCPWD)/chibios

