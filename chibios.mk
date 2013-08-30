
# require destination of this library in $(APP_LIBS)

FCPWD=$(APP_LIBS)/fc/src

# Append  all the source files.
LIBSRC += ${FCPWD}/fc.c \
${FCPWD}/proto.c \
${FCPWD}/fcseq.c \

# Add required include directories
LIBINC += ${FCPWD}

