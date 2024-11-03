#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure to hold dynamic arrays and related variables */
typedef struct {
    int arraysFilled;
    int numRecords;
    int maxRecords;

    char **stringArray;
    char **parseArray;
    int *hodnota1Array;
    float *hodnota2Array;
} RecordArrays;

/* Function to duplicate a string (since strdup is not standard in C90) */
char *my_strdup(const char *s) {
    char *d = (char *)malloc(strlen(s) + 1);
    if (d != NULL) {
        strcpy(d, s);
    }
    return d;
}

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

/* Function to count the number of lines in a file */
int countLines(FILE *file) {
    int lines = 0;
    char buffer[256];
    rewind(file);
    while (fgets(buffer, sizeof(buffer), file)) {
        lines++;
    }
    rewind(file);
    return lines;
}

/* v_command: Handle the 'v' command option */
void v_command(int option, FILE **fileString, FILE **fileData, FILE **fileParse, RecordArrays *arrays) {
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
    } else if (option == 2) {
        if (!arrays->arraysFilled) {
            printf("V2: Unfilled arrays.\n");
            return;
        }

        int i;
        for (i = 0; i < arrays->numRecords; i++) {
            printf("ID. mer. modulu: %s\n", arrays->stringArray[i]);
            printf("Hodnota 1: %d\n", arrays->hodnota1Array[i]);
            printf("Hodnota 2: %.4f\n", arrays->hodnota2Array[i]);
            printf("Poznamka: %s\n", arrays->parseArray[i]);
            printf("\n");
        }
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

    int histogram[256];
    int i;
    char c;

    /* Initialize histogram */
    for (i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

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

/* n_command: Initialize dynamic arrays with data from files */
void n_command(FILE *fileString, FILE *fileData, FILE *fileParse, RecordArrays *arrays) {
    int i;
    if (!fileString || !fileData || !fileParse) {
        printf("N: File not opened.\n");
        return;
    }

    /* Deallocate existing arrays if they exist */
    if (arrays->arraysFilled) {
        for (i = 0; i < arrays->numRecords; i++) {
            free(arrays->stringArray[i]);
            free(arrays->parseArray[i]);
        }
        free(arrays->stringArray);
        free(arrays->parseArray);
        free(arrays->hodnota1Array);
        free(arrays->hodnota2Array);

        arrays->stringArray = NULL;
        arrays->parseArray = NULL;
        arrays->hodnota1Array = NULL;
        arrays->hodnota2Array = NULL;
        arrays->numRecords = 0;
        arrays->maxRecords = 0;
        arrays->arraysFilled = 0;
    }

    /* Count lines in each file */
    int countString = countLines(fileString);
    int countData = countLines(fileData);
    int countParse = countLines(fileParse);

    /* Find the largest number of records */
    arrays->maxRecords = countString;
    if (countData > arrays->maxRecords) arrays->maxRecords = countData;
    if (countParse > arrays->maxRecords) arrays->maxRecords = countParse;

    /* Allocate arrays */
    arrays->stringArray = (char **)malloc(arrays->maxRecords * sizeof(char *));
    arrays->parseArray = (char **)malloc(arrays->maxRecords * sizeof(char *));
    arrays->hodnota1Array = (int *)malloc(arrays->maxRecords * sizeof(int));
    arrays->hodnota2Array = (float *)malloc(arrays->maxRecords * sizeof(float));

    if (!arrays->stringArray || !arrays->parseArray || !arrays->hodnota1Array || !arrays->hodnota2Array) {
        /* Handle allocation failure */
        printf("Memory allocation failed.\n");
        /* Free any allocated memory */
        if (arrays->stringArray) free(arrays->stringArray);
        if (arrays->parseArray) free(arrays->parseArray);
        if (arrays->hodnota1Array) free(arrays->hodnota1Array);
        if (arrays->hodnota2Array) free(arrays->hodnota2Array);
        arrays->arraysFilled = 0;
        return;
    }

    /* Read data from files and store into arrays */
    char buffer[256];
    char dataLine[256];
    i = 0;

    /* Reset file pointers to beginning */
    rewind(fileString);
    rewind(fileData);
    rewind(fileParse);

    while (i < arrays->maxRecords) {
        /* Read from string.txt */
        if (fgets(buffer, sizeof(buffer), fileString)) {
            buffer[strcspn(buffer, "\n")] = '\0'; /* Remove newline */
            arrays->stringArray[i] = my_strdup(buffer);
        } else {
            arrays->stringArray[i] = my_strdup("-");
        }

        /* Read from data.txt */
        if (fgets(dataLine, sizeof(dataLine), fileData)) {
            int num1, num2;
            int hodnota1 = 0;
            float hodnota2 = 0.0;
            sscanf(dataLine, "%d %d %d %f", &num1, &num2, &hodnota1, &hodnota2);
            arrays->hodnota1Array[i] = hodnota1;
            arrays->hodnota2Array[i] = hodnota2;
        } else {
            arrays->hodnota1Array[i] = 0;
            arrays->hodnota2Array[i] = 0.0;
        }

        /* Read from parse.txt */
        if (fgets(buffer, sizeof(buffer), fileParse)) {
            buffer[strcspn(buffer, "\n")] = '\0'; /* Remove newline */
            arrays->parseArray[i] = my_strdup(buffer);
        } else {
            arrays->parseArray[i] = my_strdup("-");
        }

        i++;
    }

    arrays->numRecords = arrays->maxRecords;
    arrays->arraysFilled = 1;
}

/* q_command: Add a record to the dynamic arrays */
void q_command(RecordArrays *arrays) {
    if (!arrays->arraysFilled) {
        printf("Q: Arrays are not created.\n");
        return;
    }

    int Y;
    int position;
    char inputBuffer[256];
    int num1, num2, hodnota1;
    float hodnota2;
    char *newString;
    char *newParse;
    int i;

    scanf("%d", &Y);

    /* Adjust Y to zero-based index */
    position = Y - 1;
    if (position < 0) position = 0;

    /* Read data from keyboard */
    /* Read string (equivalent to string.txt) */
    getchar(); /* Consume newline left by scanf */
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; /* Remove newline */
    newString = my_strdup(inputBuffer);

    /* Read 3 integers and 1 float */
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    sscanf(inputBuffer, "%d %d %d %f", &num1, &num2, &hodnota1, &hodnota2);

    /* Read parse string */
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; /* Remove newline */
    newParse = my_strdup(inputBuffer);

    /* Increase arrays if necessary */
    if (arrays->numRecords >= arrays->maxRecords) {
        arrays->maxRecords += 10; /* Increase capacity by 10 */
        arrays->stringArray = (char **)realloc(arrays->stringArray, arrays->maxRecords * sizeof(char *));
        arrays->parseArray = (char **)realloc(arrays->parseArray, arrays->maxRecords * sizeof(char *));
        arrays->hodnota1Array = (int *)realloc(arrays->hodnota1Array, arrays->maxRecords * sizeof(int));
        arrays->hodnota2Array = (float *)realloc(arrays->hodnota2Array, arrays->maxRecords * sizeof(float));

        if (!arrays->stringArray || !arrays->parseArray || !arrays->hodnota1Array || !arrays->hodnota2Array) {
            printf("Memory allocation failed.\n");
            return;
        }
    }

    if (position > arrays->numRecords) {
        position = arrays->numRecords;
    }

    /* Shift elements to make room */
    for (i = arrays->numRecords; i > position; i--) {
        arrays->stringArray[i] = arrays->stringArray[i -1];
        arrays->parseArray[i] = arrays->parseArray[i -1];
        arrays->hodnota1Array[i] = arrays->hodnota1Array[i -1];
        arrays->hodnota2Array[i] = arrays->hodnota2Array[i -1];
    }

    /* Insert new data */
    arrays->stringArray[position] = newString;
    arrays->parseArray[position] = newParse;
    arrays->hodnota1Array[position] = hodnota1;
    arrays->hodnota2Array[position] = hodnota2;

    arrays->numRecords++;
}

/* w_command: Delete records with given ID from dynamic arrays */
void w_command(RecordArrays *arrays) {
    if (!arrays->arraysFilled) {
        printf("W: Arrays are not created.\n");
        return;
    }

    char idToDelete[256];
    int deletedCount = 0;
    int i, j;

    /* Read the ID from the keyboard */
    scanf("%s", idToDelete);

    for (i = 0; i < arrays->numRecords; ) {
        if (strcmp(arrays->stringArray[i], idToDelete) == 0) {
            /* Delete record at position i */
            free(arrays->stringArray[i]);
            free(arrays->parseArray[i]);

            /* Shift elements to the left */
            for (j = i; j < arrays->numRecords - 1; j++) {
                arrays->stringArray[j] = arrays->stringArray[j + 1];
                arrays->parseArray[j] = arrays->parseArray[j + 1];
                arrays->hodnota1Array[j] = arrays->hodnota1Array[j + 1];
                arrays->hodnota2Array[j] = arrays->hodnota2Array[j + 1];
            }
            arrays->numRecords--;
            deletedCount++;
            /* No need to increment i, check the new element at position i */
        } else {
            i++;
        }
    }

    printf("W: Vymazalo sa : %d zaznamov !\n", deletedCount);
}

/* e_command: Search for a word in parseArray and print matching records */
void e_command(RecordArrays *arrays) {
    if (!arrays->arraysFilled) {
        printf("E: Arrays are not created.\n");
        return;
    }

    char searchWord[256];
    int i;

    scanf("%s", searchWord);

    for (i = 0; i < arrays->numRecords; i++) {
        if (strstr(arrays->parseArray[i], searchWord) != NULL) {
            printf("%s\n", arrays->parseArray[i]);
        }
    }
}

/* k_command: Handle the 'k' command for cleanup and program termination */
void k_command(FILE **fileString, FILE **fileData, FILE **fileParse, RecordArrays *arrays) {
    int i;
    /* Close any open files */
    closeFiles(fileString, fileData, fileParse);

    /* Free dynamic arrays if they exist */
    if (arrays->arraysFilled) {
        for (i = 0; i < arrays->numRecords; i++) {
            free(arrays->stringArray[i]);
            free(arrays->parseArray[i]);
        }
        free(arrays->stringArray);
        free(arrays->parseArray);
        free(arrays->hodnota1Array);
        free(arrays->hodnota2Array);

        arrays->stringArray = NULL;
        arrays->parseArray = NULL;
        arrays->hodnota1Array = NULL;
        arrays->hodnota2Array = NULL;
        arrays->numRecords = 0;
        arrays->maxRecords = 0;
        arrays->arraysFilled = 0;
    }
}

/* Main program with command handling */
int main() {
    char command;
    int option;
    FILE *fileString = NULL, *fileData = NULL, *fileParse = NULL;
    RecordArrays arrays;
    arrays.arraysFilled = 0;
    arrays.numRecords = 0;
    arrays.maxRecords = 0;
    arrays.stringArray = NULL;
    arrays.parseArray = NULL;
    arrays.hodnota1Array = NULL;
    arrays.hodnota2Array = NULL;

    while (scanf("%c", &command) != EOF) {
        if (command == 'v') {
            scanf("%d", &option);
            v_command(option, &fileString, &fileData, &fileParse, &arrays);
        } else if (command == 'h') {
            h_command(fileString);
        } else if (command == 'n') {
            n_command(fileString, fileData, fileParse, &arrays);
        } else if (command == 'q') {
            q_command(&arrays);
        } else if (command == 'w') {
            w_command(&arrays);
        } else if (command == 'e') {
            e_command(&arrays);
        } else if (command == 'k') {
            k_command(&fileString, &fileData, &fileParse, &arrays);
            break;
        } else {
            /* Handle invalid commands if necessary */
        }
    }

    return 0;
}