WDIR   = .
ODIR   = obj

CXX = g++

CXXFLAGS  += -I.  -std=c++0x

# Optimization flag
CXXFLAGS += -g #-O3

# Enable the maximun warning
#CXXFLAGS += -Wall -Wextra -Weffc++ -g

# Include ROOT
CXXFLAGS  += $(shell root-config --cflags)

CXXDEPFLAGS = -MMD -MP

LD         = g++
LDFLAGS    =

INCLUDES += $(shell $(PYTHONCFG) --includes)

LIBS      = $(shell root-config --glibs)

PROGRAMS = processPhase processGsel

all: mkobj $(PROGRAMS)

mkobj:
	@mkdir -p obj

#Working Directory 
$(ODIR)/%.o : $(WDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -o $@ -c $<

$(ODIR)/%.o : $(WDIR)/%.c
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -o $@ -c $<

$(ODIR)/%.o : $(WDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -o $@ -c $<

$(ODIR)/%.o : $(WDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS) -o $@ -c $<


processPhase: $(ODIR)/processPhase.o $(ODIR)/jsoncpp.o
	$(LD) $^ $(LIBS) -o $@

processGsel: $(ODIR)/processGsel.o $(ODIR)/jsoncpp.o
	$(LD) $^ $(LIBS) -o $@


clean:
	rm -rf $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core $(ODIR)

-include $(ODIR)/*.d
