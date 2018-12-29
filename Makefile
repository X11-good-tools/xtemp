OBJS += xtemp.c
CFLAGS += -g -O2 -fPIE -pie -D_FORTIFY_SOURCE=2 -fstack-protector-strong --param=ssp-buffer-size=1 -Weverything -Werror
LDFLAGS += -Wl,-z,relro,-z,now -lX11 -lXrandr -lm -I /usr/include/X11/ -L /usr/lib/xorg/modules/
CC = clang

xtemp: $(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o xtemp
