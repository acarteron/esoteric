HWSRC:=$(wildcard $(ROOT)/$(DIR_SRC)/hw/*.cpp)
HWOBJ:=$(HWSRC:$(ROOT)/$(DIR_SRC)/hw/%.cpp=$(ROOT)/$(DIR_OBJ)/$(TARGET)/src/hw/%.o)


# Compilation from sources (.cpp) to objects (.o)
$(ROOT)/$(DIR_OBJ)/$(TARGET)/src/hw/%.o: $(ROOT)/$(DIR_SRC)/hw/%.cpp
	@if [ ! -d $(DIR_OBJ)/$(TARGET)/src/hw ]; then mkdir -p $(DIR_OBJ)/$(TARGET)/src/hw; fi
	@echo "Compiling ... : $@"
	@$(GXX) -c -o $@ $< $(CFLAGS_OBJ) $(CFLAGS)
