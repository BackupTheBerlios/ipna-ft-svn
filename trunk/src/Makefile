MAJOR_VERSION=0
MINOR_VERSION=5
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION)

INCLUDE_DIR=../include/ipna/

LDFLAGS=-L.
LDLIBS=-lboost_program_options -lboost_filesystem -lQtCore -lQtNetwork #-lipnaftcore
CXXFLAGS=-O3 -fPIC -g -I../include -I/usr/include/qt4 -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork

PREFIX=/home/ipna

SRCS=network/Listener.cc network/SequenceNumberChecker.cc network/Packet.cc network/HostPort.cc IPNAProgram.cc Logger.cc
CORE_HDRS=$(addprefix $(INCLUDE_DIR), $(SRCS:.cc=.hpp))
CORE_OBJS=$(SRCS:.cc=.o)
CORE_LIB=libipnaftcore.so

FANOUT_SRCS=fanout/FanoutProgram.cc fanout/FanoutPacketHandler.cc
FANOUT_HDRS=$(addprefix $(INCLUDE_DIR), $(FANOUT_SRCS:.cc=.hpp))

FANOUT_MAIN=fanout/main.cc
FANOUT_OBJS=$(FANOUT_MAIN:.cc=.o) $(FANOUT_SRCS:.cc=.o) $(CORE_OBJS)
FANOUT_BIN=ipna-ft-fanout

FANOUT_LIB_OBJS=$(FANOUT_SRCS:.cc=.o)
FANOUT_LIB=libipnaftfanout.so

PARSER_SRCS=parser/ParserFactory.cc parser/CNFPv9PacketParser.cc parser/Field.cc
PARSER_HDRS=$(addprefix $(INCLUDE_DIR), $(PARSER_SRCS:.cc=.hpp))
PARSER_OBJS=$(PARSER_SRCS:.cc=.o)

PARSER_LIB_OBJS=$(PARSER_OBJS)
PARSER_LIB=libipnaftparser.so

CAPTURE_SRCS=capture/CaptureProgram.cc capture/CapturePacketHandler.cc capture/FileRecordWriter.cc capture/RecordWriter.cc capture/Formatter.cc
CAPTURE_HDRS=$(addprefix $(INCLUDE_DIR), $(CAPTURE_SRCS:.cc=.hpp))
CAPTURE_MAIN=capture/main.cc
CAPTURE_OBJS=$(CAPTURE_MAIN:.cc=.o) $(CAPTURE_SRCS:.cc=.o) $(PARSER_SRCS:.cc=.o) $(SRCS:.cc=.o)
CAPTURE_BIN=ipna-ft-capture

CAPTURE_LIB_OBJS=$(CAPTURE_SRCS:.cc=.o)
CAPTURE_LIB=libipnaftcapture.so

BINS=$(FANOUT_BIN) $(CAPTURE_BIN)
OBJS=$(FANOUT_OBJS) $(CAPTURE_OBJS) $(PARSER_OBJS)
LIBS=$(CORE_LIB) $(PARSER_LIB) $(CAPTURE_LIB) $(FANOUT_LIB)

.PHONY: default all fanout capture clean install test libs init

%.o : %.c
	@echo "[CC ] $<"
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.cc
	@echo "[C++] $<"
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

% :
	@echo "[LD ] $^ => $@"
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.so :
	@echo "[LIB] $^ => $@"
	@$(CC) -shared -Wl,-soname,$@.$(VERSION) -o $@.$(VERSION) $^
	@ln -s $@.$(VERSION) $@

default: all

all: init libs fanout capture

init:


libs: #$(LIBS)
fanout: $(FANOUT_BIN)
capture: $(CAPTURE_BIN)

$(CORE_LIB): $(CORE_OBJS)
$(PARSER_LIB): $(PARSER_LIB_OBJS)
$(FANOUT_LIB): $(FANOUT_LIB_OBJS)
$(CAPTURE_LIB): $(CAPTURE_LIB_OBJS)

$(FANOUT_OBJS): $(CORE_HDRS) $(FANOUT_HDRS)
$(CAPTURE_OBJS): $(CORE_HDRS) $(PARSER_HDRS) $(CAPTURE_HDRS)

$(FANOUT_BIN) : $(FANOUT_OBJS)
$(CAPTURE_BIN): $(CAPTURE_OBJS)

install: $(BINS)
	cp -p $(BINS) $(PREFIX)/bin

clean:
	-rm $(OBJS) 2>/dev/null
	-rm $(BINS) 2>/dev/null
	-rm $(LIBS) 2>/dev/null
	-rm *.so.* 2>/dev/null

test:
	./ipna-ft-capture -v -l */5100

