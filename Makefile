
CC := gcc
CFLAGS := -Wall -Wextra -ansi -pedantic -Iinclude -O2 -g
LDLIBS := -lm -pthread
LDFLAGS :=

MKDIR := mkdir -p
DOXYGEN := doxygen

BUILDDIR = _build
DEPDIR = $(BUILDDIR)/deps

#############################################################################

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: install clean docs test

install:
	@echo "NYI!"

docs:
	$(DOXYGEN) Doxyfile

clean:
	$(RM) $(BUILDDIR)/*.o $(DEPDIR)/*.d

test: $(BUILDDIR)/alloc-bench
	$(BUILDDIR)/alloc-bench

#############################################################################

$(BUILDDIR)/%.o: src/%.c
$(BUILDDIR)/%.o: src/%.c $(DEPDIR)/%.d | $(BUILDDIR) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: tests/%.c
$(BUILDDIR)/%.o: tests/%.c $(DEPDIR)/%.d | $(BUILDDIR) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILDDIR)/alloc-bench: $(BUILDDIR)/alloc-bench.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
$(BUILDDIR)/logging-test: $(BUILDDIR)/logging.o $(BUILDDIR)/logging-test.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
$(BUILDDIR)/vector-test: $(BUILDDIR)/logging.o $(BUILDDIR)/vector-test.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

DEPFILES = $(patsubst src/%.c,$(DEPDIR)/%.d,$(wildcard src/*.c))     \
		   $(patsubst tests/%.c,$(DEPDIR)/%.d,$(wildcard tests/*.c))
$(DEPFILES):
-include $(DEPFILES)

$(BUILDDIR):
	$(MKDIR) $@
$(DEPDIR):
	$(MKDIR) $@



