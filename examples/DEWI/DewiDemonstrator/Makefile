CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = example-dewi
LDFLAGS+=-lm
CFLAGS+=-lm
TARGET_LIBFILES+=-lm

MODULES += core/net/DEWI/rll

all: $(CONTIKI_PROJECT)

CONTIKI = ../../..
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
