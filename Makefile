#
# Determine OS
#
# You won't need to change this.
#

Uname := $(shell uname)

#
# Platform-specific compilation flags.
#
# Use this to specify different libraries for bingsuns vs. Linux.
#

ifeq ($(Uname),Linux)
	LDLIBS :=
endif
ifeq ($(Uname),SunOS)
	LDLIBS := -lsocket -lnsl -lrt
endif

#
# Optimization/debug flag.
# Change this to -O and recompile when doing performance testing.
#

OptFlag = -g

#
# Code generator files
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
CodeGenSrcs := \
	cg.cpp \
	Emitter.cpp \

#
# RMI library files
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
LibSrcs := \
	Skeleton.cpp \
	Params.cpp \
	Rmi.cpp \
	RmiServer.cpp \

#
# Development interfaces
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
DevInterfaces := \
	DevInterface \

#
# Development client files.
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
DevClientProgSrcs := \
	dev_client.cpp \

#
# Development server files.
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
DevServerProgSrcs := \
	dev_server.cpp \

#
# Evaluation interfaces
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
EvalInterfaces := \
	EvalInterfaceBasic \
	EvalInterfaceManyMethods \
	EvalInterfaceConcurrency \

#
# Evaluation client files.
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
EvalClientProgSrcs := \
	eval_client.cpp \

#
# Evaluation server files.
#

# DON'T DELETE THE BLANK LINE AT THE END OF THIS LIST.
EvalServerProgSrcs := \
	eval_server.cpp \

################################################################################
######### You probably do not need to change anything below this line. #########
################################################################################

# File and other useful variable aggregation -----------------------------------

#
# Code generator files.
#

CodeGenObjs := $(CodeGenSrcs:.cpp=.o)

#
# RMI library files.
#

LibObjs := $(LibSrcs:.cpp=.o)

#
# Development client files.
#

# *Prog{Hdrs,Srcs} means hand-programmed files.
# *Gen{Hdrs,Srcs} means generated files.
DevClientGenHdrs := $(foreach i,$(DevInterfaces),$(i).hpp $(i)_stub.hpp)
DevClientGenSrcs := $(foreach i,$(DevInterfaces),$(i).cpp $(i)_stub.cpp)

DevClientSrcs := $(DevClientProgSrcs) $(DevClientGenSrcs)
DevClientObjs := $(DevClientSrcs:.cpp=.o)

#
# Development server files, and joint files.
#

DevServerGenHdrs := $(foreach i,$(DevInterfaces),$(i).hpp $(i)_skel.hpp)
DevServerGenSrcs := $(foreach i,$(DevInterfaces),$(i).cpp $(i)_skel.cpp)

DevServerSrcs := $(DevServerProgSrcs) $(DevServerGenSrcs)
DevServerObjs := $(DevServerSrcs:.cpp=.o)

DevGenHdrs := $(sort $(DevClientGenHdrs) $(DevServerGenHdrs))
DevGenSrcs := $(sort $(DevClientGenSrcs) $(DevServerGenSrcs))

#
# Evaluation client files.
#

# *Prog{Hdrs,Srcs} means hand-programmed files.
# *Gen{Hdrs,Srcs} means generated files.
EvalClientGenHdrs := $(foreach i,$(EvalInterfaces),$(i).hpp $(i)_stub.hpp)
EvalClientGenSrcs := $(foreach i,$(EvalInterfaces),$(i).cpp $(i)_stub.cpp)

EvalClientSrcs := $(EvalClientProgSrcs) $(EvalClientGenSrcs)
EvalClientObjs := $(EvalClientSrcs:.cpp=.o)

#
# Evaluation server files, and joint files.
#

EvalServerGenHdrs := $(foreach i,$(EvalInterfaces),$(i).hpp $(i)_skel.hpp)
EvalServerGenSrcs := $(foreach i,$(EvalInterfaces),$(i).cpp $(i)_skel.cpp)

EvalServerSrcs := $(EvalServerProgSrcs) $(EvalServerGenSrcs)
EvalServerObjs := $(EvalServerSrcs:.cpp=.o)

EvalGenHdrs := $(sort $(EvalClientGenHdrs) $(EvalServerGenHdrs))
EvalGenSrcs := $(sort $(EvalClientGenSrcs) $(EvalServerGenSrcs))

#
# Aggregates.
#

GenHdrs := $(DevGenHdrs) $(EvalGenHdrs)
GenSrcs := $(DevGenSrcs) $(EvalGenSrcs)
GenFiles := $(GenHdrs) $(GenSrcs)

Srcs := $(CodeGenSrcs) $(LibSrcs) $(DevClientSrcs) $(DevServerSrcs) $(EvalClientSrcs) $(EvalServerSrcs)
Deps := $(patsubst %,.%.do,$(basename $(Srcs)))

# Compilation flags ------------------------------------------------------------

CPPFLAGS = -MMD -MF $(basename .$(@F)).do
CXXFLAGS := -Wall -Wno-unused $(OptFlag)
ifeq ($(Uname),Linux)
CXXFLAGS += -pthread
RunPathOption = -Wl,-rpath=$(CURDIR)
endif
ifeq ($(Uname),SunOS)
CXXFLAGS += -pthreads
RunPathOption = -Wl,-R,$(CURDIR)
endif

# Target for all ---------------------------------------------------------------

.PHONY: all
all: cg librmi.so dev_client dev_server eval_client eval_server
# echo GENFILES: $(GenFiles)
# echo DEPS: $(Deps)

# cg target --------------------------------------------------------------------

cg: $(CodeGenObjs)
	g++ $(CXXFLAGS) -o cg $(CodeGenObjs)

# librmi.so target -------------------------------------------------------------

librmi.so: CXXFLAGS += -fPIC
librmi.so: $(LibObjs)
	g++ -shared $(CXXFLAGS) -o librmi.so $(LibObjs) $(LDLIBS)

# dev_client and dev_server target ---------------------------------------------

dev_client: $(DevClientObjs) librmi.so
	g++ $(CXXFLAGS) $(RunPathOption) -o dev_client $^ $(LDLIBS)

$(DevClientObjs): $(DevClientGenHdrs)

dev_server: $(DevServerObjs) librmi.so
	g++ $(CXXFLAGS) $(RunPathOption) -o dev_server $^ $(LDLIBS)

$(DevServerObjs): $(DevServerGenHdrs)

# eval_client and eval_server target ---------------------------------------------

eval_client: $(EvalClientObjs) librmi.so
	g++ $(CXXFLAGS) $(RunPathOption) -o eval_client $^ $(LDLIBS)

$(EvalClientObjs): $(EvalClientGenHdrs)

eval_server: $(EvalServerObjs) librmi.so
	g++ $(CXXFLAGS) $(RunPathOption) -o eval_server $^ $(LDLIBS)

$(EvalServerObjs): $(EvalServerGenHdrs)

# Miscellaneous dependencies ---------------------------------------------------

$(GenFiles): cg

%.hpp %.cpp %_stub.hpp %_stub.cpp %_skel.hpp %_skel.cpp: %.idl
	./cg $<

# Include dependencies ---------------------------------------------------------

-include $(Deps)

# clean target -----------------------------------------------------------------

.PHONY: clean
clean:
	rm -f cg librmi.so dev_client dev_server eval_client eval_server *.o $(GenFiles) .*.do core core.*
