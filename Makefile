#
# Makefile for Asterisk PicoTTS application
#
# This program is free software, distributed under the terms of
# the GNU General Public License Version 2. See the COPYING file
# at the top of the source tree.


INSTALL=install
ASTLIBDIR:=$(shell awk '/moddir/{print $$3}' /etc/asterisk/asterisk.conf)
ifeq ($(strip $(ASTLIBDIR)),)
        MODULES_DIR=$(INSTALL_PREFIX)/usr/lib/asterisk/modules
else
        MODULES_DIR=$(INSTALL_PREFIX)$(ASTLIBDIR)
endif
ASTETCDIR=$(INSTALL_PREFIX)/etc/asterisk

CC=gcc
OPTIMIZE=-O2
DEBUG=-g

LIBS+=
CFLAGS+= -pipe -fPIC -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -D_REENTRANT  -D_GNU_SOURCE  -DAST_MODULE=\"res_mwi_devstate\"

all: _all
	@echo " +---- res_mwi_devstate Build Complete ----+"  
	@echo " + res_mwi_devstate successfully built,    +"  
	@echo " + and can be installed by running:        +"
	@echo " +                                         +"
	@echo " +               make install              +"  
	@echo " +-----------------------------------------+" 

_all: res_mwi_devstate.so


res_mwi_devstate.o: res_mwi_devstate.c
	$(CC) $(CFLAGS) $(DEBUG) $(OPTIMIZE) -c -o res_mwi_devstate.o res_mwi_devstate.c

res_mwi_devstate.so: res_mwi_devstate.o
	$(CC) -shared -Xlinker -x -o $@ $< $(LIBS)


clean:
	rm -f res_mwi_devstate.o res_mwi_devstate.so .*.d *.o *.so *~

install: _all
	$(INSTALL) -m 755 -d $(DESTDIR)$(MODULES_DIR)
	$(INSTALL) -m 755 res_mwi_devstate.so $(DESTDIR)$(MODULES_DIR)
	
	@echo " +------------ res_mwi_devstate -----------+"  
	@echo " +                                         +"
	@echo " + res_mwi_devstate successfully installed +"  
	@echo " +                                         +"
	@echo " +-----------------------------------------+"

