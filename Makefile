
#################################
#     Overridable variables     #
#################################
CFLAGS?=-O2
LFLAGS?=
XDO_LFLAGS?=-lxdo
XDO_INCLUDES?=
X11_LFLAGS?=$(shell pkg-config --libs x11)
X11_INCLUDES?=$(shell pkg-config --cflags x11 xkbcommon)


OUTPUT=kpmouse
LIBS=$(XDO_LFLAGS) $(X11_LFLAGS) -lm
INCLUDES+=$(XDO_INCLUDES) $(X11_INCLUDES)

# These flags will later be used to keep track of which .h files imported
# by each compilation unit. Teh goal is making make rebuild affected
# units after cahnges in headers. Source:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPFLAGS=-MT $@ -MMD -MP -MF build/$*.Td

# Each .c file in src will yield a compilation unit
SOURCES=$(wildcard src/*.c)
OBJS:=$(patsubst %.c,build/%.o,$(SOURCES))

# Targets which always run (no checking changes in deps)
.PHONY: all submission clean

# Create build dir, before trying to access it
$(shell mkdir -p build/src >/dev/null)

# default target
all: build/kpmouse

# "Erases" the rule
%.o: %.c

# Build rule for each compilation unit.
# The .Td -> .d move is a workaround to avoid gcc kill leaving corrupted
# dependency files. Since the .d file is written after .o, touching the .o
# is necessary to keep make from thinking that the .o needs rebuilding forever.
build/%.o : %.c build/%.d
	$(CC) -Wall -Werror -std=c99 $(CFLAGS) $(INCLUDES) $(DEPFLAGS) -o $@ -c $<
	mv -f build/$*.Td build/$*.d && touch $@

# Link compilation units
build/kpmouse: $(OBJS)
	$(CC) -Wall -Werror -std=c99 $(CFLAGS) $(LFLAGS) -o $@ $^ $(LIBS)
	cp build/kpmouse $(OUTPUT)

# Clean build files and the output binary
clean:
	rm -fr build $(OUTPUT)


# Inform make that .d files are targets (not only deps). This avoids failing the first build
build/%.d: ;

# Do not treat .d files as intermediary files.
.PRECIOUS: build/%.d

# Parse all commands in the .d files as make commands, establishing
# .c -> .h dependencies
include $(wildcard $(patsubst %,build/%.d,$(basename $(SOURCES))))

