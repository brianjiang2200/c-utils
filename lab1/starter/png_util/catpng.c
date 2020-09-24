#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab_png.h"

int main(int argc, char** argv) {
	if (argc == 1) {
		return -1;
	}
	/*assumed all arguments are valid pngs*/
	/*ALGORITHM TO GENERATE NEW IDAT DATA
		Get compressed pixel data from each png, inflate the data, and store as new data
		Take new data and create IDAT chunk by compressing using mem_def
	*/
	FILE* merged = fopen("all.png", "w");
	/*write data to merged png in order
	HEADER: CAN COPY FROM THE FIRST FILE
	IHDR:
		LENGTH IS 13
		TYPE CAN COPY
		DATA: ALL SAME, EXCEPT HEIGHT IS THE SUM OF THE HEIGHTS OF THE CONSTITUENTS
		CRC: COMPUTED BASED ON NEW TYPE AND DATA
	IDAT: Use the above algorithm to generate this field
	IEND: CAN COPY FROM FIRST FILE
	*/
	fclose(merged);
	return 0;
}
