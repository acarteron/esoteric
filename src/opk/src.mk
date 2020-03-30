OPKSRC:=$(wildcard $(ROOT)/$(DIR_SRC)/opk/*.cpp)
OPKOBJ:=$(OPKSRC:$(ROOT)/$(DIR_SRC)/opk/%.cpp=$(ROOT)/$(DIR_OBJ)/$(TARGET)/src/opk/%.o)


# Compilation from sources (.cpp) to objects (.o)
$(ROOT)/$(DIR_OBJ)/$(TARGET)/src/opk/%.o: $(ROOT)/$(DIR_SRC)/opk/%.cpp
	@if [ ! -d $(DIR_OBJ)/$(TARGET)/src/opk ]; then mkdir -p $(DIR_OBJ)/$(TARGET)/src/opk; fi
	@echo "Compiling ... : $@"
	@$(GXX) -c -o $@ $< $(CFLAGS_OBJ) $(CFLAGS)
