OBJ 	= hwbug
#ARCH    = PPC
ARCH    = arm

MEN_LIN_DIR=/opt/menlinux

HWBUG_DIR=$(MEN_LIN_DIR)/TOOLS/HWBUG

FILES	= hwbug.o linux.o change.o display.o history.o

MAK_SWITCH = -DLINUX\
		-DMAC_IO_MAPPED_EN\

CC=gcc

CFLAGS	= -g -s $(MAK_SWITCH) -I$(MEN_LIN_DIR)/INCLUDE/COM -I=. -I/usr/src/linux-4.9.0/include

WARN_LEVEL := -Wall -Wno-format -Wno-strict-aliasing

%.o : %.c  MEN/typdefs.h
	echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):	$(FILES) $(MEN_LIN_DIR)/LIB/$(ARCH)/MEN/libuti.a
	echo Compiling $@
	$(CC) $(WARN_LEVEL) \
		$(FILES) $(MEN_LIN_DIR)/LIB/$(ARCH)/MEN/libuti.a \
		-g -o $@

clean: clean_hwbug

clean_hwbug:
	rm -rf $(HWBUG_DIR)/*.o $(HWBUG_DIR)/hwbug
