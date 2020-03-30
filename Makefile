include ./standard_def.mk

EXEC=$(ROOT)/$(DIR_BIN)/$(TARGET)/esoteric

all: createdirectories $(EXEC) 

createdirectories:
	@mkdir -p $(DIR_OBJ)/$(TARGET)/src $(DIR_BIN)/$(TARGET) $(DIR_LIB) $(DIR_DIST) $(DIR_OPK)

-include $(ROOT)/$(DIR_SRC)/src.mk


.PHONY: clean doc lib

lib:	createdirectories $(LIBR)

# ifeq ($(MAKECMDGOALS),lib)
# -include $(ROOT)/$(DIR_SRC)/libsrc.mk
# endif


doc:
	@echo -n "Generation de la DOC ... "
	@mkdir -p $(DIR_DOC)
 
	@doxygen 
	@echo "OK."

clean:
	@echo "Cleaning Project"
	@rm -rf $(DIR_OBJ) $(DIR_BIN) $(DIR_ASM) #$(DIR_DOC)

clean-asm:
	@echo "Cleaning ASM"
	@rm -rf $(DIR_ASM) 

raz: 
	@echo "Cleaning Project++"
	@rm -rf $(DIR_OBJ) $(DIR_LIB) $(DIR_BIN) $(DIR_DOC) $(DIR_DIST) $(DIR_OPK) 
