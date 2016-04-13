DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = example-dewi


MODULES += core/net/mac/tsch
MODULES += core/net/DEWI/cider
MODULES += core/net/DEWI/rll
MODULES += core/net/DEWI/scheduler
all: $(CONTIKI_PROJECT)

CONTIKI = ../..
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
