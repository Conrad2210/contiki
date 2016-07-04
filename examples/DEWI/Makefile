CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = example-dewi
LDFLAGS+=-lm
CFLAGS+=-lm
TARGET_LIBFILES+=-lm

MODULES += core/net/DEWI/cider
MODULES += core/net/DEWI/rll
MODULES += core/net/DEWI/scheduler
MODULES += core/net/DEWI/neighTable
MODULES += core/net/mac/tsch

all: $(CONTIKI_PROJECT)

CONTIKI = ../..
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
