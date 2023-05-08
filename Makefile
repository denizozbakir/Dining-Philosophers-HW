CC=gcc
DEPS=meal.h
LIB=-pthread

make: kitchen.c meal.h
	$(CC) -o kitchen $(DEPS) $(LIB) kitchen.c

clean:
	rm -f kitchen

