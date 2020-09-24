#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab_png.h"
#include "crc.h"
#include "zutil.h"

int main(int argc, char** argv) {
	if (argc == 1) {
		return -1;
	}
	/*assumed all arguments are valid pngs*/

	FILE* sample = fopen(argv[1], "r");
	if (sample == NULL) {
		return -1;
	}
	/*read header*/
	U8 header[8];
	fread(header, 8, 1, sample);
	/*read IHDR data*/
	struct chunk* new_IHDR = malloc(sizeof(struct chunk));
	get_chunk(new_IHDR, sample, 0);
	/*read IEND data*/
	struct chunk* new_IEND = malloc(sizeof(struct chunk));
	get_chunk(new_IEND, sample, 2);

	FILE* merged = fopen("all.png", "w");
	fwrite(header, 1, 8, merged);

	free(new_IHDR);
	free(new_IEND);
	fclose(merged);
	return 0;
}

/*ALGORITHM TO GENERATE NEW IDAT DATA
	Get compressed pixel data from each png, inflate the data, and store as new data
	Take new data and create IDAT chunk by compressing using mem_def
*/

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
