nogui:
	gcc -std=gnu99 -g -Wall -o mlcc mlcc.c

gui:
	gcc -std=gnu99 -g -Wall -o mlcc mlcc.c -DGUI `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

clean:
	rm mlcc
