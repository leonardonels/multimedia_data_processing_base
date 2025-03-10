// incapsulation, inheritance and polymorphism

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

// at least 24 bytes for a vector
typedef struct vector
{
    size_t size, capacity;
    int *data;
} vector;
// a constructor will allocates memory fot our object, initialising all internal stuff...
void vector_construct(struct vector *this)
{
    this->size = 0;
    this->capacity = 1;
    this->data = NULL;
    this->data = malloc(this->capacity * sizeof(int));
    if (this->data == NULL)
    {
        perror("Memory allocation error!");
        exit(EXIT_FAILURE); //EXIT_FAILURE == 1
    }
}

void vector_destroy(struct vector *this)
{
    free(this->data);
}

void vector_push_back(struct vector *this, int x)
{
    if (this->size == this->capacity)
    {
        this->capacity *= 2;
        int *new_data = (int *)realloc(this->data, this->capacity * sizeof(int));
        if (new_data == NULL)
        {
            perror("Memory allocation error!");
            exit(EXIT_FAILURE);
        }
        this->data = new_data;
    }
    this->data[this->size] = x;
    this->size++;
}

size_t vector_size(struct vector const *this)
{
    return this->size;
}

int vector_at(struct vector const *this, size_t index)
{
    assert(index < this->size); // works only in debug mode, in release mode will be ignored
    return this->data[index];
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

    struct vector numbers;
    vector_construct(&numbers);

    int num;
    while (fscanf(filein, "%d", &num) != EOF)
    {
        vector_push_back(&numbers, num);
    }

    qsort(numbers.data, numbers.size, sizeof(int), compare_ints);

    for (int i = 0; i < vector_size(&numbers); i++) {
        fprintf(fileout, "%d\n", vector_at(&numbers, i));
    }

    fclose(filein);
    fclose(fileout);
    vector_destroy(&numbers);

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
