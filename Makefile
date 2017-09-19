# New Makefile that automatically depends itself
#
# $Id: Makefile,v 1.3 1996/12/17 19:52:37 chase Exp $
#

IFLAGS = 
DFLAGS = 
CXX = gcc
CC  = $(GCC)
GCC = gcc
LD  = $(CXX)

LIBS = 

WFLAGS = -Wall 
SYMFLAGS = -g

PROFILE = #-pg 
OPTFLAGS =#-O
CFLAGS = $(OPTFLAGS) $(PROFILE) $(WFLAGS) $(IFLAGS) $(SYMFLAGS)
CXXFLAGS = $(CFLAGS)
CPPFLAGS = $(IFLAGS) $(DFLAGS)
LDFLAGS = $(PROFILE) -g 

PROGRAM = yash
#CXXSRCS = Source.cpp

CXXSRCS = $(shell ls *.cpp)
CSRCS = $(shell ls *.c)          

SRCS = $(CSRCS) 

OBJS = $(CSRCS:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

test: $(PROGRAM)
	./$(PROGRAM)

clean:
	-rm -f $(OBJS) $(PROGRAM)

tidy:
	-rm -f *.BAK *.bak *.CKP

undepend:
	-rm -f $(OBJS:%.o=.%.d) 

spotless: tidy clean undepend

.y.c:
	$(BISON) $(BISONFLAGS) -o $@ $<
	mv $@.h $*.h
	mv $@.output $*.output
.l.c:
	$(FLEX) ${FLEXFLAGS} -t $< > $@

# auto depend stuff for GNU make only
depend: undepend
	@echo ""
	@echo "Dependences are handled automatically, just \"make\""

ifneq ($(strip $(CSRCS)),)
.%.d: %.c 
	$(SHELL) -ec '$(GCC) -MM $(CFLAGS) $< > $@'


include $(CSRCS:%.c=.%.d)
endif 

ifneq ($(strip $(CSRCS)),)
.%.d: %.c
	$(SHELL) -ec '$(GCC) -MM $(CFLAGS) $< > $@'

include $(CSRCS:%.c=.%.d) 
endif 


