SRC := #
SRC += main.c mplayer_gtk.c othermain.c  lyrics_process.c

#OBJ := $(subst .c,.o,$(SRC))
OBJ = $(SRC:%.c=%.o)

export PKG_CONFIG_PATH=/opt/gtkdfb/lib/pkgconfig
PREFIX = /opt/gtkdfb
LDFLAGS=-L${PREFIX}/lib -Wl,-rpath,${PREFIX}/lib 
CFLAGS=-I${PREFIX}/include/gtk-2.0/ 

CC = arm-linux-gcc
FLAG = -Wall $(LDFLAGS) $(CFLAGS) `pkg-config --cflags --libs gtk+-2.0` 
OPTION = -lpthread -ldl  -lgthread-2.0
EXEC_NAME = demo
EXEC_PATH = .

.PHONY:clean demo

demo:$(OBJ)
	@echo make ...
	$(CC) $^ -o $(EXEC_PATH)/$(EXEC_NAME) $(FLAG) $(OPTION)
	@echo make over
	@echo Execute target is $(EXEC_PATH)/$(EXEC_NAME)
$(OBJ):%.o:%.c
	$(CC) -c -o $@ $< $(FLAG)
clean:
	@echo clean ...
	rm $(EXEC_PATH)/$(EXEC_NAME) *.o -rf
	@echo clean over
	