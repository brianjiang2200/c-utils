pnginfo: lab_png.h crc.h lab_png.c crc.c pnginfo.c
	gcc -std=c99 lab_png.c crc.c pnginfo.c -o pnginfo

findpng: lab_png.h crc.h lab_png.c crc.c findpng.c
	gcc -std=c99 lab_png.c crc.c findpng.c -o findpng

catpng: lab_png.h crc.h zutil.h lab_png.c crc.c zutil.c catpng.c
	gcc -std=c99 lab_png.c crc.c zutil.c catpng.c -o catpng -lz

run: catpng
	valgrind ./catpng ../images/cropped/pic_cropped_0.png ../images/cropped/pic_cropped_1.png ../images/cropped/pic_cropped_2.png ../images/cropped/pic_cropped_3.png ../images/cropped/pic_cropped_4.png
	valgrind ./catpng ../images/uweng_cropped/uweng_cropped_0.png ../images/uweng_cropped/uweng_cropped_1.png ../images/uweng_cropped/uweng_cropped_2.png ../images/uweng_cropped/uweng_cropped_3.png ../images/uweng_cropped/uweng_cropped_4.png ../images/uweng_cropped/uweng_cropped_5.png ../images/uweng_cropped/uweng_cropped_6.png

test: catpng
	./catpng output_0.png output_2.png output_3.png output_4.png output_5.png output_6.png output_8.png output_9.png output_10.png output_11.png output_12.png output_14.png output_15.png output_16.png output_18.png output_19.png output_21.png output_22.png output_23.png output_24.png output_25.png output_26.png output_27.png output_28.png output_30.png output_31.png output_32.png output_33.png

clean:
	rm -f catpng pnginfo findpng
