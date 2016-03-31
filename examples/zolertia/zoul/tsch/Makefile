DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = test-tsch test-tsch-coordinator
MODULES += core/net/mac/tsch
all: $(CONTIKI_PROJECT)

CONTIKI = ../../../..
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
