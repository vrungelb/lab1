#include "functions.h"

#define LINE_BUFFER_SIZE 128


static char *read_line(char *buffer, size_t size) {
    // читаем строку длиной до size−1 символов из stdin
    if (fgets(buffer, (int)size, stdin) == NULL) {
        // если fgets вернула NULL - значит встретился EOF или ошибка чтения
        return NULL;
    }

    // мерим длину полученной строки
    size_t len = strlen(buffer);
    // если последний символ — '\n', заменяем его на '\0'
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    // возвращаем указатель на буфер с прочитанной (и обрезанной) строкой
    return buffer;
}



int getInt(int *a) {
    char  buffer[LINE_BUFFER_SIZE];
    long  tmp;
    char *endptr;

    while (1) {
        if (read_line(buffer, sizeof(buffer)) == NULL) {
            /* EOF или ошибка чтения */
            fprintf(stderr, "\033[1;31mError: expected an integer, but got EOF or read error.\n\033[0m");
            return -1;
        }

        tmp = strtol(buffer, &endptr, 10); /* тут 10 означает, что strtol
                                            конвертирует строку в десятичную
                                            систему счисления

        /* endptr укажет на первый символ,
        который не вошёл в число */
        if (endptr == buffer) {
            fprintf(stderr, "\033[1;31mInvalid input. Please enter a valid integer: \033[0m");
            continue;
        }
        /* Если после цифр в строке что-то осталось */
        if (*endptr != '\0') {
            fprintf(stderr, "\033[1;31mExtra characters detected. Enter only an integer: \033[0m");
            continue;
        }
        /* Чекаем, чтобы попало в диапазон int */
        if (tmp < INT_MIN || tmp > INT_MAX) {
            fprintf(stderr, "\033[1;31mNumber out of range [%d..%d]. Try again: \033[0m", INT_MIN, INT_MAX);
            continue;
        }

        *a = (int)tmp;
        return 0;
    }
}



int create(Matrix *M) {
    int tmp;

    // Запрашиваем у пользователя кол-во строк матрицы
    printf("\033[1;33mEnter number of rows: \033[0m");
    if (getInt(&tmp) < 0)
        return -1;

    if (tmp <= 0) {
        fprintf(stderr, "\033[1;31mNumber of rows must be positive.\n\033[0m");
        return -1;
    }
    M->lines = tmp;

    // Выделяем память под массив структур Line размером lines
    M->pline = (Line *)malloc(sizeof(Line) * M->lines);
    if (M->pline == NULL) {
        fprintf(stderr, "\033[1;31mMemory allocation failed for %zu rows.\n\033[0m", M->lines);
        return -1;
    }

    // Инициализировать каждую строку: 0 элементов и указатель NULL
    for (int i = 0; i < M->lines; i++) {
        M->pline[i].elements = 0;
        M->pline[i].pval     = NULL;
    }
    return 0;
}



int columns(const Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        return -1;
    }
    return M->pline[row].elements;
}


/* Возвращает указатель на структуру Line для заданной строки, проверяя, не вышли ли за пределы матрицы*/
Line *indTask(Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        return NULL;
    }
    return &M->pline[row];
}



int inputColForRow(Matrix *M, int row) {
    // Проверить корректность индекса строки
    if (row < 0 || row >= M->lines) {
        fprintf(stderr, "\033[1;31mInvalid row index %d in inputColForRow.\n\033[0m", row + 1);
        return -1;
    }

    int tmp;
    // Запрос у пользователя количества элементов в строке row+1
    printf("\033[1;33mEnter number of elements in row %d: \033[0m", row + 1);
    if (getInt(&tmp) < 0) {
        return -1;
    }
    // Кол-во элементов не может быть отрицательным
    if (tmp < 0) {
        fprintf(stderr, "\033[1;31mNumber of elements must be non-negative.\n\033[0m");
        return -1;
    }
    // сохраняем tmp в поле elements структуры Line
    M->pline[row].elements = tmp;
    return 0;
}



int allocMemForRow(Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        fprintf(stderr, "\033[1;31mInvalid row index %d in allocMemForRow.\n\033[0m", row + 1);
        return -1;
    }

    int n = M->pline[row].elements;

    if (n <= 0) {
        M->pline[row].pval = NULL;
        return 0;
    }

    M->pline[row].pval = malloc(sizeof(int) * n);
    if (M->pline[row].pval == NULL) {
        fprintf(stderr, "\033[1;31mMemory allocation failed for row %d with %d elements.\n\033[0m", row + 1, n);
        return -1;
    }
    return 0;
}



int inputValForRow(Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        fprintf(stderr, "Invalid row index %d in inputValForRow.\n", row + 1);
        return -1;
    }

    int n = M->pline[row].elements;
    if (n <= 0) {
        return 0;
    }
    // Для каждого элемента запрашиваем у пользователя значение
    for (int j = 0; j < n; j++) {
        int tmpInt;
        printf("\033[1;33mEnter value [%d][%d]: \033[0m", row + 1, j + 1);
        if (getInt(&tmpInt) < 0) {
            return -1;
        }
        M->pline[row].pval[j] = tmpInt;
    }
    return 0;
}



void printLine(const Line *L) {
    printf("[");
    for (int j = 0; j < L->elements; j++) {
        if (j > 0) 
            printf("  ");
        printf("%d", L->pval[j]);
    }
    printf("]\n");
}



void printMatrix(const Matrix *M) {
    // Заголовок перед выводом матрицы
    printf("\n\033[1;32mYOUR MATRIX:\033[0m\n");
    for (int i = 0; i < M->lines; i++) {
        // Печатаем номер строки (i+1 для удобства)
        printf("Row %d: ", i + 1);
        // Выводим i-ю строку с помощью printLine
        printLine(&M->pline[i]);
    }
    // Пустая строка после матрицы
    printf("\n");
}



void eraseLine(Line *L) {
    if (L->pval != NULL) {
        free(L->pval);
        L->pval = NULL;
    }
    L->elements = 0;
}



void eraseMatrix(Matrix *M) {
    if (M->pline != NULL) {
        for (int i = 0; i < M->lines; i++) {
            eraseLine(&M->pline[i]);
        }
        // Освобождаем сам массив структур Line
        free(M->pline);
        M->pline = NULL;
        M->lines = 0;
    }
}


// Вводит матрицу M, запрашивая у юзера кол-во элементов в каждой строке
int inputMatrix(Matrix *M) {
    for (int i = 0; i < M->lines; i++) {
        if (inputColForRow(M, i) != 0) {
            eraseMatrix(M);
            return -1;
        }
        if (allocMemForRow(M, i) != 0) {
            eraseMatrix(M);
            return -1;
        }
        if (inputValForRow(M, i) != 0) {
            eraseMatrix(M);
            return -1;
        }
    }
    return 0;
}


// Вычисляет сам итоговый вектор b
int computeVectorB(const Matrix *M, Vector *b_out) {
    size_t n = M->lines;
    b_out->size = n;

    b_out->data = malloc(sizeof(long) * n);
    if (b_out->data == NULL) {
        fprintf(stderr, "\033[1;31mMemory allocation failed for vector b.\n\033[0m");
        return -1;
    }

    for (size_t i = 0; i < n; i++) {
        long sum_even = 0;
        long sum_odd  = 0;
        Line L = M->pline[i];

        for (size_t j = 0; j < L.elements; j++) {
            int v = L.pval[j];
            if ((v % 2) == 0) sum_even += v;
            else            sum_odd  += v;
        }
        b_out->data[i] = sum_even - sum_odd;
    }
    return 0;
}



void printVectorB(const Vector *b) {
    printf("\033[1;32mRESULTING VECTOR b:\033[0m\n");
    for (int i = 0; i < b->size; i++) {
        printf("b[%d] = %ld\n", i + 1, b->data[i]);
    }
}
