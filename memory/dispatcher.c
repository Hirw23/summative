#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "data.txt"
#define MAX_BUFFER 1024

// Function declarations
void add_data();
void count_lines();
void count_characters();
void to_uppercase();
void to_lowercase();
void view_file();

void display_menu();

void (*dispatcher[])(void) = {
    add_data,
    count_lines,
    count_characters,
    to_uppercase,
    to_lowercase,
    view_file
};

int main() {
    int choice;

    while (1) {
        display_menu();

        if (scanf("%d", &choice) != 1 || choice < 0 || choice > 6) {
            printf("Invalid input. Please enter a number between 0 and 6.\n");
            while (getchar() != '\n'); // clear input buffer
            continue;
        }

        if (choice == 6) {
            printf("Exiting...\n");
            break;
        }

        dispatcher[choice]();
    }

    return 0;
}

// Function implementations

void display_menu() {
    printf("\nDynamic Function Dispatcher Menu:\n");
    printf("0. Add data to file\n");
    printf("1. Count lines in file\n");
    printf("2. Count characters in file\n");
    printf("3. Convert file content to UPPERCASE\n");
    printf("4. Convert file content to lowercase\n");
    printf("5. View file content\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
}

void add_data() {
    FILE *fp = fopen(FILENAME, "a");
    if (!fp) {
        perror("Error opening file for appending");
        return;
    }

    char *buffer = (char *)malloc(MAX_BUFFER);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return;
    }

    printf("Enter data to add to file: ");
    while (getchar() != '\n'); // clear newline
    fgets(buffer, MAX_BUFFER, stdin);
    fprintf(fp, "%s", buffer);

    printf("Data added to file.\n");

    free(buffer);
    fclose(fp);
}

void count_lines() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    int lines = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') lines++;
    }

    printf("Number of lines in file: %d\n", lines);
    fclose(fp);
}

void count_characters() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    int count = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        count++;
    }

    printf("Number of characters in file: %d\n", count);
    fclose(fp);
}

void to_uppercase() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Error opening file for reading");
        return;
    }

    char *buffer = (char *)malloc(MAX_BUFFER);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return;
    }

    int i = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF && i < MAX_BUFFER - 1) {
        buffer[i++] = toupper(ch);
    }
    buffer[i] = '\0';
    fclose(fp);

    fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("Error opening file for writing");
        free(buffer);
        return;
    }

    fprintf(fp, "%s", buffer);
    printf("File content converted to UPPERCASE.\n");

    free(buffer);
    fclose(fp);
}

void to_lowercase() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Error opening file for reading");
        return;
    }

    char *buffer = (char *)malloc(MAX_BUFFER);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return;
    }

    int i = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF && i < MAX_BUFFER - 1) {
        buffer[i++] = tolower(ch);
    }
    buffer[i] = '\0';
    fclose(fp);

    fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("Error opening file for writing");
        free(buffer);
        return;
    }

    fprintf(fp, "%s", buffer);
    printf("File content converted to lowercase.\n");

    free(buffer);
    fclose(fp);
}

void view_file() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Error opening file to view content");
        return;
    }

    char ch;
    printf("\n--- File Content ---\n");
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }
    printf("\n--- End of File ---\n");

    fclose(fp);
}

