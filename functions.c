#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define LINE_BUFFER_SIZE 128

/*================ Диалоговые функции ================*/

/* Читает строку из stdin, удаляет завершающий '\n' */
static char *read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL)
        return NULL;
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = '\0';
    return buffer;
}

/* Считывает целое число, выводит сообщения об ошибках, возвращает 0 или –1 */
int getInt(int *a) {
    char  buffer[LINE_BUFFER_SIZE];
    long  tmp;
    char *endptr;

    while (1) {
        if (read_line(buffer, sizeof(buffer)) == NULL) {
            fprintf(stderr, "\033[1;31mОшибка: ожидалось целое число, но получен EOF или ошибка чтения.\n\033[0m");
            return -1;
        }
        tmp = strtol(buffer, &endptr, 10);
        if (endptr == buffer) {
            fprintf(stderr, "\033[1;31mНеверный ввод. Введите корректное целое число: \033[0m");
            continue;
        }
        if (*endptr != '\0') {
            fprintf(stderr, "\033[1;31mОбнаружены лишние символы. Введите только целое число: \033[0m");
            continue;
        }
        if (tmp < INT_MIN || tmp > INT_MAX) {
            fprintf(stderr, "\033[1;31mЧисло вне диапазона [%d..%d]. Попробуйте снова: \033[0m", INT_MIN, INT_MAX);
            continue;
        }
        *a = (int)tmp;
        return 0;
    }
}

/* Запрашивает число строк, выделяет M->pline и инициализирует элементы */
int create(Matrix *M) {
    int tmp;
    printf("\033[1;33mВведите число строк: \033[0m");
    if (getInt(&tmp) < 0) return -1;
    if (tmp <= 0) {
        fprintf(stderr, "\033[1;31mЧисло строк должно быть положительным.\n\033[0m");
        return -1;
    }
    M->lines = tmp;
    M->pline = malloc(sizeof(Line) * M->lines);
    if (M->pline == NULL) {
        fprintf(stderr, "\033[1;31mНе удалось выделить память для %d строк.\n\033[0m", M->lines);
        return -1;
    }
    for (int i = 0; i < M->lines; i++) {
        M->pline[i].elements = 0;
        M->pline[i].pval     = NULL;
    }
    return 0;
}

/* Запрашивает число элементов в указанной строке и сохраняет в M->pline[row].elements */
int inputColForRow(Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        fprintf(stderr, "\033[1;31mНеверный индекс строки %d в inputColForRow.\n\033[0m", row+1);
        return -1;
    }
    int tmp;
    printf("\033[1;33mВведите число элементов в строке %d: \033[0m", row+1);
    if (getInt(&tmp) < 0) return -1;
    if (tmp < 0) {
        fprintf(stderr, "\033[1;31mЧисло элементов не может быть отрицательным.\n\033[0m");
        return -1;
    }
    M->pline[row].elements = tmp;
    return 0;
}

/* Запрашивает значения элементов в строке и сохраняет в M->pline[row].pval */
int inputValForRow(Matrix *M, int row) {
    if (row < 0 || row >= M->lines) {
        fprintf(stderr, "\033[1;31mНеверный индекс строки %d в inputValForRow.\n\033[0m", row+1);
        return -1;
    }
    int n = M->pline[row].elements;
    for (int j = 0; j < n; j++) {
        int tmp;
        printf("\033[1;33mВведите значение [%d][%d]: \033[0m", row+1, j+1);
        if (getInt(&tmp) < 0) return -1;
        M->pline[row].pval[j] = tmp;
    }
    return 0;
}

/* Последовательно заполняет всю матрицу: размеры строк и их элементы */
int inputMatrix(Matrix *M) {
    for (int i = 0; i < M->lines; i++) {
        if (inputColForRow(M, i) < 0)  { eraseMatrix(M); return -1; }
        if (allocMemForRow(M, i)  < 0) { eraseMatrix(M); return -1; }
        if (inputValForRow(M, i)  < 0) { eraseMatrix(M); return -1; }
    }
    return 0;
}

/* Выводит одну строку в формате [v0  v1  ...] */
void printLine(const Line *L) {
    printf("[");
    for (int j = 0; j < L->elements; j++) {
        if (j) printf("  ");
        printf("%d", L->pval[j]);
    }
    printf("]\n");
}

/* Выводит всю матрицу построчно */
void printMatrix(const Matrix *M) {
    printf("\n\033[1;32mВАША МАТРИЦА:\033[0m\n");
    for (int i = 0; i < M->lines; i++) {
        printf("Строка %d: ", i+1);
        printLine(&M->pline[i]);
    }
    printf("\n");
}

/* Выводит результирующий вектор b */
void printVectorB(const Vector *b) {
    printf("\033[1;32mРЕЗУЛЬТИРУЮЩИЙ ВЕКТОР b:\033[0m\n");
    for (int i = 0; i < b->size; i++) {
        printf("b[%d] = %ld\n", i+1, b->data[i]);
    }
}

/*================ Функции без I/O ================*/

/* Возвращает число столбцов в строке или –1 при ошибочном индексе */
int columns(const Matrix *M, int row) {
    if (!M || row < 0 || row >= M->lines) return -1;
    return M->pline[row].elements;
}

/* Возвращает указатель на структуру Line или NULL при неверном индексе */
Line *indTask(Matrix *M, int row) {
    if (!M || row < 0 || row >= M->lines) return NULL;
    return &M->pline[row];
}

/* Выделяет память под элементы строки, возвращает 0 или –1 */
int allocMemForRow(Matrix *M, int row) {
    if (!M || row < 0 || row >= M->lines) return -1;
    int n = M->pline[row].elements;
    if (n <= 0) {
        M->pline[row].pval = NULL;
        return 0;
    }
    M->pline[row].pval = malloc(sizeof(int) * n);
    return M->pline[row].pval ? 0 : -1;
}

/* Освобождает память одной строки и обнуляет её поля */
void eraseLine(Line *L) {
    if (!L) return;
    free(L->pval);
    L->pval     = NULL;
    L->elements = 0;
}

/* Освобождает всю матрицу и обнуляет её поля */
void eraseMatrix(Matrix *M) {
    if (!M || !M->pline) return;
    for (int i = 0; i < M->lines; i++)
        eraseLine(&M->pline[i]);
    free(M->pline);
    M->pline = NULL;
    M->lines = 0;
}

/* Вычисляет b[i] = сумма чётных – сумма нечётных, возвращает 0 или –1 */
int computeVectorB(const Matrix *M, Vector *b_out) {
    if (!M || !b_out) return -1;
    int   n = M->lines;
    long *buf = malloc(sizeof(long) * n);
    if (!buf) return -1;
    for (int i = 0; i < n; i++) {
        long se = 0, so = 0;
        Line L = M->pline[i];
        for (int j = 0; j < L.elements; j++) {
            int v = L.pval[j];
            if ((v & 1) == 0) se += v;
            else             so += v;
        }
        buf[i] = se - so;
    }
    b_out->data = buf;
    b_out->size = n;
    return 0;
}
