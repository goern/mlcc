nogui:
	gcc -std=gnu99 -g -Wall -o mlcc mlcc.c

gui:
	gcc -std=gnu99 -g -Wall -o mlcc mlcc.c -DGUI `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

clean:
	rm -f mlcc
	rm -rf /tmp/mlcc-0.1.0

src: clean
	mkdir /tmp/mlcc-0.1.0
	cp mlcc.c /tmp/mlcc-0.1.0
	cp Makefile /tmp/mlcc-0.1.0
	cp LICENSE /tmp/mlcc-0.1.0
	cd /tmp && tar -cvzf mlcc-0.1.0.tar.gz mlcc-0.1.0
	mv /tmp/mlcc-0.1.0.tar.gz ~/rpmbuild/SOURCES/
