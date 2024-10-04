RACK_DIR ?= ../..

SOURCES += NotStraightLines.cpp

DISTRIBUTABLES += $(wildcard LICENSE*) res

include $(RACK_DIR)/plugin.mk
