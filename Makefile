include ./standard_def.mk

ifeq ($(target),rg-350-vga)
-include ./standard_def_rg350vga.mk
endif

ifeq ($(target),linux)
-include ./standard_def_linux.mk
endif

ifeq ($(target), )
-include ./standard_def_rg350vga.mk
endif

EXEC=$(ROOT)/$(DIR_BIN)/$(TARGET)/esoteric

all: dist

dist: createdirectories $(EXEC) 
	install -m755 -D $(EXEC) $(DIR_DST)/$(TARGET)/esoteric
	install -m644 -D README.md $(DIR_DST)/$(TARGET)/esoteric.man.txt
	install -m644 -D COPYING $(DIR_DST)/$(TARGET)/COPYING
	install -m644 -D ChangeLog.md $(DIR_DST)/$(TARGET)/ChangeLog.md
	install -m644 -D about.txt $(DIR_DST)/$(TARGET)/about.txt
	install -m644 -D assets/$(target)/esoteric.conf $(DIR_DST)/$(TARGET)
	cp -RH assets/$(target)/skins assets/translations $(DIR_DST)/$(TARGET)
	cp -RH assets/$(target)/scripts $(DIR_DST)/$(TARGET)
	cp -RH assets/$(target)/sections $(DIR_DST)/$(TARGET)
	cp -RH assets/input $(DIR_DST)/$(TARGET)
	cp assets/$(target)/icons/rg350.png $(DIR_DST)/$(TARGET)/logo.png
	cp assets/$(target)/icons/rg350.png $(DIR_DST)/$(TARGET)/skins/Default/icons/device.png
	cp assets/$(target)/icons/rg350.png $(DIR_DST)/$(TARGET)/skins/Minimal/icons/device.png

# all: createdirectories $(EXEC) 

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
	@rm -rf $(DIR_OBJ) $(DIR_LIB) $(DIR_BIN) $(DIR_DOC) $(DIR_DIST) $(DIR_OPK) $(DIR_DST) 
