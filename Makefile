CC ?= clang
CFLAGS += -g -O2 -fPIE -pie -D_FORTIFY_SOURCE=2 -fstack-protector-strong --param=ssp-buffer-size=1 -Werror
LDFLAGS += -Wl,-z,relro,-z,now -lX11 -lXrandr -lm -I /usr/include/X11/ -L /usr/lib/xorg/modules/
TARGET = xtemp
OBJS += src/$(TARGET).c
RM ?= rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)