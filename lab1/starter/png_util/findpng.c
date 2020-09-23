#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab_png.h"

void check_directory(char* d_name, unsigned int* png_count) {
	DIR* p_dir;
	struct dirent *p_dirent;
	char str[64];

	/*if directory cannot be opened, simply return*/
	if ((p_dir = opendir(d_name)) == NULL) {
		return;
	}

	while ((p_dirent = readdir(p_dir)) != NULL) {
		char *str_path = p_dirent->d_name;

		if (str_path == NULL) {
			return;
		} else {
			if (p_dirent->d_type == 4) {
                                /*check directories are not "." and ".."*/
                                if (strcmp(str_path, ".") != 0 && strcmp(str_path, "..") != 0) {                                                         /*recursively check subfolders*/
                                }
                        }
                        /*else if potential png candidate found*/
                        if (p_dirent->d_type == 8) {
                                /*check first if valid png*/
                                printf("%s\n", str_path);
                                png_count++;
                        }
		}
	}

}

int main(int argc, char** argv) {
	DIR *p_dir;
	struct dirent *p_dirent;
	char str[64];

	if (argc == 1) {
		fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
		exit(1);
	}

	if ((p_dir = opendir(argv[1])) == NULL) {
		sprintf(str, "opendir(%s)", argv[1]);
		perror(str);
		exit(2);
	}

	unsigned int png_count = 0;

	/*keep reading directory contents*/
	while ((p_dirent = readdir(p_dir)) != NULL) {
		char *str_path = p_dirent->d_name;

		if (str_path == NULL) {
			exit(3);
		} else {
			/*if directory is found*/
			if (p_dirent->d_type == 4) {
				/*check directories are not "." and ".."*/
				if (strcmp(str_path, ".") != 0 && strcmp(str_path, "..") != 0) {
					/*recursively check subfolders*/
				}
			}
			/*else if potential png candidate found*/
			if (p_dirent->d_type == 8) {
				/*check first if valid png*/
				printf("%s\n", str_path);
				png_count++;
			}
		}
	}

	/*if empty search result*/
	if (!png_count) {
		printf("findpng: No PNG file found\n");
	}

	if (closedir(p_dir) != 0) {
		perror("closedir");
		exit(3);
	}

	return 0;
}
