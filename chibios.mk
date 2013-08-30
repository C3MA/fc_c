
# require destination of this library in $(APP_LIBS)

FCPWD=$(APP_LIBS)/fc/src

# List of all the ChibiOS/RT test files.
TESTSRC = ${FCPWD}/fc.c \
${FCPWD}/proto.c \
${FCPWD}/fcseq.c \

# Required include directories
TESTINC = ${FCPWD}

