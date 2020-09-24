pnginfo: lab_png.h crc.h lab_png.c crc.c pnginfo.c
	gcc -std=c99 lab_png.c crc.c pnginfo.c -o pnginfo

findpng: lab_png.h crc.h lab_png.c crc.c findpng.c
	gcc -std=c99 lab_png.c crc.c findpng.c -o findpng

catpng: lab_png.h crc.h zutil.h lab_png.c crc.c zutil.c catpng.c
	gcc -std=c99 lab_png.c crc.c zutil.c catpng.c -o catpng -lz

run: catpng
	./catpng ../images/red-green-16x16.png

clean:
	rm -f catpng
