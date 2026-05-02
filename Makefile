CC = clang
# Hem M serisi (homebrew) hem Intel Mac yollarini manuel olarak ekliyoruz
CFLAGS = -Wall -Wextra -I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lSDL2

SRC = main.c
OUT = bilardo_oyunu.out

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)