#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    int id;
    char name[50];
    int age;
    char course[50];
    float grade;
} Student;

Student students[MAX];
int count = 0;

// Helper to clear input buffer
void clear_input() {
    while (getchar() != '\n');
}

// Validate integer input
int get_valid_int(const char *prompt) {
    int num;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &num) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input();
        } else {
            clear_input();
            return num;
        }
    }
}

// Validate float input
float get_valid_float(const char *prompt) {
    float num;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &num) != 1) {
            printf("Invalid input. Please enter a valid number.\n");
            clear_input();
        } else {
            clear_input();
            return num;
        }
    }
}

void add_student() {
    if (count >= MAX) {
        printf("Student limit reached.\n");
        return;
    }

    students[count].id = get_valid_int("Enter ID: ");
    printf("Enter Name: ");
    fgets(students[count].name, sizeof(students[count].name), stdin);
    students[count].name[strcspn(students[count].name, "\n")] = '\0';  // Remove newline
    students[count].age = get_valid_int("Enter Age: ");
    printf("Enter Course: ");
    fgets(students[count].course, sizeof(students[count].course), stdin);
    students[count].course[strcspn(students[count].course, "\n")] = '\0';
    students[count].grade = get_valid_float("Enter Grade: ");

    count++;
    printf("Student added successfully!\n");
}

void display_students() {
    if (count == 0) {
        printf("No student records found.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        printf("\nID: %d\nName: %s\nAge: %d\nCourse: %s\nGrade: %.2f\n",
               students[i].id, students[i].name, students[i].age, students[i].course, students[i].grade);
    }
}

void search_student() {
    int id = get_valid_int("Enter ID to search: ");
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            printf("Student found:\nID: %d\nName: %s\nAge: %d\nCourse: %s\nGrade: %.2f\n",
                   students[i].id, students[i].name, students[i].age, students[i].course, students[i].grade);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found.\n");
    }
}

void update_student() {
    int id = get_valid_int("Enter ID of student to update: ");
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            printf("Updating record for %s...\n", students[i].name);
            printf("Enter new name: ");
            fgets(students[i].name, sizeof(students[i].name), stdin);
            students[i].name[strcspn(students[i].name, "\n")] = '\0';
            students[i].age = get_valid_int("Enter new age: ");
            printf("Enter new course: ");
            fgets(students[i].course, sizeof(students[i].course), stdin);
            students[i].course[strcspn(students[i].course, "\n")] = '\0';
            students[i].grade = get_valid_float("Enter new grade: ");
            printf("Student updated successfully.\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found.\n");
    }
}

void delete_student() {
    int id = get_valid_int("Enter ID of student to delete: ");
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            for (int j = i; j < count - 1; j++) {
                students[j] = students[j + 1];
            }
            count--;
            printf("Student deleted successfully.\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found.\n");
    }
}

void sort_students() {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(students[j].name, students[j + 1].name) > 0) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    printf("Students sorted by name.\n");
}

void save_to_file() {
    FILE *fp = fopen("students.txt", "w");
    if (!fp) {
        printf("Error saving to file.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d,%s,%d,%s,%.2f\n", students[i].id, students[i].name, students[i].age,
                students[i].course, students[i].grade);
    }
    fclose(fp);
}

void load_from_file() {
    FILE *fp = fopen("students.txt", "r");
    if (!fp) return;

    while (fscanf(fp, "%d,%49[^,],%d,%49[^,],%f\n",
                  &students[count].id,
                  students[count].name,
                  &students[count].age,
                  students[count].course,
                  &students[count].grade) == 5) {
        count++;
    }
    fclose(fp);
}

int main() {
    load_from_file();
    int choice;

    do {
        printf("\n--- Student Management System ---\n");
        printf("1. Add Student\n");
        printf("2. View Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Sort Students by Name\n");
        printf("7. Save & Exit\n");

        choice = get_valid_int("Enter your choice: ");

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_students(); break;
            case 3: search_student(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6: sort_students(); break;
            case 7:
                save_to_file();
                printf("Data saved. Exiting...\n");
                break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 7);

    return 0;
}

