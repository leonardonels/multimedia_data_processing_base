#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_NUMBERS 10000
#define MAX_CHARS 100

int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: sort_int <filein.txt> <fileout.txt>\n");
        return 1;
    }

    FILE *filein = fopen(argv[1], "r");
    if (!filein) {
        perror("Error opening input file");
        return 1;
    }

    int numbers[MAX_NUMBERS];
    char str[MAX_CHARS];
    int count = 0;

    char ch;
    int i = 0;
    while ((ch = fgetc(filein)) != EOF)
    {
        if (isdigit(ch) || ch == '-')
        {
            str[i++] = ch;
        }
        else if (isspace(ch) || ch == '\n')
        {
            if (i>0)
            {
                str[i] = '\0';
                numbers[count++] = atoi(str);
                i=0;
            }
        }else
        {
            if (i>0)
            {
                str[i] = '\0';
                numbers[count++] = atoi(str);
                i=0;
            }
            break;
        }
    }

    fclose(filein);

    qsort(numbers, count, sizeof(int), compare_ints);

    FILE *fileout = fopen(argv[2], "w");
    if (!fileout) {
        perror("Error opening output file");
        fclose(filein);
        return 1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fileout, "%d\n", numbers[i]);
    }

    fclose(fileout);

    return 0;
}






/*
#define MAX_NUMBERS 10000
#define MAX_LINE_LENGTH 16

int is_valid_integer(const char *str) {
    if (*str == '-' || *str == '+') {
        str++;
    }

    if (*str == '\0') return 0;

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: sort_int <filein.txt> <fileout.txt>\n");
        return 1;
    }

    FILE *filein = fopen(argv[1], "r");
    if (!filein) {
        perror("Error opening input file");
        return 1;
    }

    FILE *fileout = fopen(argv[2], "w");
    if (!fileout) {
        perror("Error opening output file");
        fclose(filein);
        return 1;
    }

    int numbers[MAX_NUMBERS];
    int count = 0;

    char ch;
    char current_token[MAX_LINE_LENGTH];

    int token_idx = 0;
    int stop_reading = 0;

    while ((ch = fgetc(filein)) != EOF) {
        if (stop_reading) {
            break;
        }

        if (isdigit(ch) || ch == '-' || ch == '+') {
            current_token[token_idx++] = ch;
            if (token_idx >= MAX_LINE_LENGTH - 1) {
                current_token[MAX_LINE_LENGTH - 1] = '\0';
                fprintf(stderr, "Token exceeded max length: %s\n", current_token);
                token_idx = 0;
                continue;
            }
        } else if (isspace(ch) || ch == '\n') {
            if (token_idx > 0) {
                current_token[token_idx] = '\0';
                if (is_valid_integer(current_token)) {
                    numbers[count++] = atoi(current_token);
                    if (count >= MAX_NUMBERS) {
                        break;
                    }
                } else {
                    fprintf(stderr, "Invalid token encountered: %s. Stopping reading.\n", current_token);
                    stop_reading = 1;
                    break;
                }
                token_idx = 0;
            }
        } else {
            if (token_idx > 0) {
                current_token[token_idx] = '\0'; // Null-terminate the token
                if (is_valid_integer(current_token)) {
                    numbers[count++] = atoi(current_token);
                    if (count >= MAX_NUMBERS) {
                        break;
                    }
                }
            }

            fprintf(stderr, "Invalid token encountered: %s. Stopping reading.\n", current_token);
            stop_reading = 1;
            break;
        }
    }

    qsort(numbers, count, sizeof(int), compare_ints);

    for (int i = 0; i < count; i++) {
        fprintf(fileout, "%d\n", numbers[i]);
    }

    fclose(filein);
    fclose(fileout);

    return 0;
}

 */