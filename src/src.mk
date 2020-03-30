# SRC:=$(wildcard $(ROOT)/$(DIR_SRC)/*.cpp)
SRC:=$(filter-out $(SRC_IGNORE) , $(wildcard $(ROOT)/$(DIR_SRC)/*.cpp))
OBJ:=$(SRC:$(ROOT)/$(DIR_SRC)/%.cpp=$(ROOT)/$(DIR_OBJ)/%.o)

ifeq ($(SRC_IGNORE), false)
-include $(ROOT)/$(DIR_SRC)/opk/src.mk
endif
-include $(ROOT)/$(DIR_SRC)/hw/src.mk
# -include $(ROOT)/$(DIR_SRC)/raw/src.mk
# -include $(ROOT)/$(DIR_SRC)/abstract/src.mk
# -include $(ROOT)/$(DIR_SRC)/main/src.mk

# Compilation of the program
# $(EXEC):  $(OBJ) #$(ALGE) $(SQL) $(SQL3)  $(MAIN)
# 	@echo "Linking ..... : $@"
# 	@$(GXX) -o $@ $^ $(CFLAGS) #$(ROOT)/$(DIR_ASM)/*.s 

$(EXEC): $(EXEC)-debug
	$(STRIP) $< -o $@

$(EXEC)-debug: $(HWOBJ) $(OPKSRC) $(OBJ)
	@echo "Linking debug..."
	$(GXX) -o $@ $^ $(LDFLAGS) 
	 # $(LDFLAGS) 

# Compilation from sources (.cpp) to objects (.o)
$(ROOT)/$(DIR_OBJ)/%.o: $(ROOT)/$(DIR_SRC)/%.cpp
	@echo "Compiling ... : $@"
	@$(GXX) -c -o $@ $<  $(CFLAGS_OBJ) $(CFLAGS)


