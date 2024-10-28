#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function to open files and return success status */
int openFiles(FILE **fileString, FILE **fileData, FILE **fileParse) {
    *fileString = fopen("string.txt", "r");
    *fileData = fopen("data.txt", "r");
    *fileParse = fopen("parse.txt", "r");

    if (!*fileString || !*fileData || !*fileParse) {
        return 0;  /* Return 0 if any file fails to open */
    }
    return 1;  /* Return 1 if all files are opened successfully */
}

/* Function to close files if open */
void closeFiles(FILE **fileString, FILE **fileData, FILE **fileParse) {
    if (*fileString) fclose(*fileString);
    if (*fileData) fclose(*fileData);
    if (*fileParse) fclose(*fileParse);
    *fileString = *fileData = *fileParse = NULL;
}

/* v_command: Handle the 'v' command option */
void v_command(int option, FILE **fileString, FILE **fileData, FILE **fileParse) {
    if (option == 1) {
        if (!*fileString || !*fileData || !*fileParse) {
            if (!openFiles(fileString, fileData, fileParse)) {
                printf("V1: Neotvorene txt subory.\n");
                return;
            }
        }

        char stringLine[100], dataLine[100], parseLine[100];

        while (fgets(stringLine, sizeof(stringLine), *fileString) &&
               fgets(dataLine, sizeof(dataLine), *fileData) &&
               fgets(parseLine, sizeof(parseLine), *fileParse)) {
            
            int hodnota1;
            float hodnota2;
            
            sscanf(dataLine, "%*d %*d %d %f", &hodnota1, &hodnota2);

            printf("ID. mer. modulu: %s", stringLine);
            printf("Hodnota 1: %d\n", hodnota1);
            printf("Hodnota 2: %.4f\n", hodnota2);
            printf("Poznamka: %s\n", parseLine);
        }

        printf("\n");
    } else {
        printf("V: Nespravne volba vypisu.\n");
    }
}

/* h_command: Handle the 'h' command option */
void h_command(FILE *fileString) {
    if (!fileString) {
        printf("H: Neotvoreny subor.\n");
        return;
    }

    int histogram[256] = {0};  /* Create a histogram for ASCII characters */
    char c;

    rewind(fileString);  /* Reset file pointer to the beginning */

    while ((c = fgetc(fileString)) != EOF) {
        if (c != '\n')  /* Exclude newline characters */
            histogram[(unsigned char)c]++;
    }

    /* Display histogram for uppercase letters (A-Z), lowercase (a-z), and digits (0-9) */
    for (c = 'A'; c <= 'Z'; c++) {
        if (histogram[(unsigned char)c] > 0) {
            printf("%c : %d\n", c, histogram[(unsigned char)c]);
        }
    }
    for (c = 'a'; c <= 'z'; c++) {
        if (histogram[(unsigned char)c] > 0) {
            printf("%c : %d\n", c, histogram[(unsigned char)c]);
        }
    }
    for (c = '0'; c <= '9'; c++) {
        if (histogram[(unsigned char)c] > 0) {
            printf("%c : %d\n", c, histogram[(unsigned char)c]);
        }
    }
}

/* k_command: Handle the 'k' command for cleanup and program termination */
void k_command(FILE **fileString, FILE **fileData, FILE **fileParse) {
    /* Close any open files */
    closeFiles(fileString, fileData, fileParse);
}

/* Main program with command handling */
int main() {
    char command;
    int option;
    FILE *fileString = NULL, *fileData = NULL, *fileParse = NULL;

    while (scanf(" %c", &command) != EOF) {
        if (command == 'v') {
            scanf("%d", &option);
            v_command(option, &fileString, &fileData, &fileParse);
        } else if (command == 'h') {
            h_command(fileString);
        } else if (command == 'k') {
            k_command(&fileString, &fileData, &fileParse);
            break;
        } 
    }

    return 0;
}