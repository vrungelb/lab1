#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* 
 *   elements — кол-во элементов,
 *   pval     — указатель на массив
*/
typedef struct Line {
    size_t elements;
    int *pval;
} Line;

/*
 *   lines — кол-во строк,
 *   pline — указатель на массив из Line
 */
typedef struct Matrix {
    size_t lines;
    Line *pline;
} Matrix;

/*
 *   Vector объединяет размер и указатель на данные типа long.
 *   size — длина массива,
 *   data — сам массив.
 */
typedef struct Vector {
    size_t size;
    long  *data;
} Vector;

/*
 *  Читает из stdin одно целое (int). 
 *  При ошибке: не число, лишние символы, EOF — вывести сообщение в поток stderr и
 *  повторять запрос до тех пор, пока не будет корректный int или EOF.
 *  Возвращает 0 при корректном чтении (занёс число в *a), 
 *  или –1 при EOF или невозможности прочитать дальше
 */
int getInt(int *a);

/*
 *  спрашивает кол-во строк матрицы, читает m через getInt,
 *  проверяет m>0,
 *  выделяет M->pline = malloc(m * sizeof(Line)),
 *  присваивает каждому M->pline[i].elements = 0; M->pline[i].pval = NULL;
 *  кладёт M->lines = m;
 *  Возвращает 0 при успехе, –1 при ошибке
 */
int create(Matrix *M);

/*
 *  возвращает, сколько элементов в строке row 
 *  Если row < 0 или row >= M->lines, возвращает –1
 */
int columns(const Matrix *M, int row);

/*
 *  возвращает указатель на M->pline[row], если row корректен, иначе NULL
 */
Line *indTask(Matrix *M, int row);

/*
 *  спрашивает «Enter number of elements in row %d:», кладёт в M->pline[row].elements.
 *  Проверяет, что row входит в [0..M->lines–1], и что введённое число >= 0.
 *  Возвращает 0 при успехе, –1 при ошибке (индекс row некорректен или ввод невалиден).
 */
int inputColForRow(Matrix *M, int row);

/*
 *  смотрит n = M->pline[row].elements, если n>0, выделяет 
 *  M->pline[row].pval = malloc(n * sizeof(double));
 *  если n==0, кладёт pval = NULL.
 *  Возвращает 0 при успехе, –1 при ошибке malloc или при неверном row.
 */
int allocMemForRow(Matrix *M, int row);

/*
 *  предполагается, что allocMemForRow уже сделал malloc, 
 *  и теперь pval указывает на массив длины elements
 *  Проходит j=0..n–1, печатает «Enter value [%d][%d]: » и вызывает getInt(&x),
 *  а затем сохраняет x в M->pline[row].pval[j]
 *  Возвращает 0 при успехе, –1 при EOF/ошибке getInt или неверном row
 */
int inputValForRow(Matrix *M, int row);

/*
 *  печатает строку L
 */
void printLine(const Line *L);

/*
 *  печатает по строкам матрицу M
 */
void printMatrix(const Matrix *M);

/*
 *  освобождает free(L->pval) и обнуляет L->elements=0, L->pval=NULL
 */
void eraseLine(Line *L);

/* 
 *  разбирает всю матрицу: для каждой строки вызывает eraseLine, 
 *  а затем free(M->pline) и M->lines=0
 */
void eraseMatrix(Matrix *M);

/*
 *  Вводит все строки матрицы M:
 *  для i от 0 до M->lines–1: вызывает inputColForRow, allocMemForRow, inputValForRow.
 *  Возвращает 0 при успехе или –1 при ошибке (и сам освобождает уже выделенную память).
 */
int inputMatrix(Matrix *M);

/*
 *  Вычисляет результирующий вектор b длины M->lines.
 *  b[i] = (сумма четных элементов i-й строки) – (сумма нечетных элементов i-й строки).
 *  Выделяет память под массив long размером M->lines и возвращает через параметр b_out.
 *  Возвращает 0 при успехе, –1 при malloc-ошибке.
 */
int computeVectorB(const Matrix *M, Vector *b_out);

/*
 *  Печатает вектор b длины size: «b[1] = …», «b[2] = …» и т. д.
 */
void printVectorB(const Vector *b);

#endif
