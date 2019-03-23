/*Задание 1.
 * В прямоугольной матрице с произвольными ненулевыми числами уровнять число отрицательных и положительных элементов
 * путем конвертации минимального числа элементов.
 * Пример на множестве: Дано: -5,6,7,8,-4,4,2,8. Получаем: -5,-6,7,8,-4,4,2,-8
 * Задание 2.
 * В полученной матрице (по результатам выполнения задания 1) найти п роизведение элементов в диапазоне [-3,3] не равных нулю.*/

#include <stdio.h>
#include "mpi.h"
#include <random>
#include <math.h>
#include "iostream"
#include <time.h>
#include <unistd.h>

#define ROW 10
#define COL 10
using namespace std;

int random_int() {
    random_device rd{};
    default_random_engine generator(rd());
    uniform_int_distribution<int> distribution(1, 100);
    return distribution(generator);
}

int main(int argc, char **argv) {
    int rank, size;
    int matrix[ROW][COL];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int balance = 0, result = 1;


    if (rank == 0) {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                matrix[i][j] = random_int() * (int) pow(-1, random_int());

            }
        }
        cout << "the matrix created!" << endl;

        //вывод матрицы в консоль

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                printf("%d%s", matrix[i][j], " ");

            }
            cout << endl;
        }

        cout << endl;
        //отправка строк по процессам
        for (int i = 1; i < size; i++) {
            for (int j = i - 1; j < ROW; j += size - 1) {
                cout << "send row " << j << ": ";
                for (int k = 0; k < COL; k++) {
                    cout << matrix[j][k] << " ";
                }
                cout << "to process " << i << endl;
                MPI_Send(matrix[j], COL, MPI_INT, i, 0,
                         MPI_COMM_WORLD);

            }

        }
        cout << endl;
        //получение баланса от процессов и вычисление общего баланса
        int received_balance;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&received_balance, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            cout << "received balance from process " << i << " is " << received_balance << endl;
            balance += received_balance;
        }
        cout << endl;
        cout << "total balance is " << balance << endl;

        //конвертация
        int count = abs(balance);


        for (int i = 0; i < ROW && count > 0; i++) {
            for (int j = 0; j < COL && count > 0; j++) {
                if ((balance < 0 && matrix[i][j] < 0) or (balance > 0 && matrix[i][j] > 0)) {
                    cout << matrix[i][j] << " changed to ";

                    matrix[i][j] = matrix[i][j] * (-1);
                    cout << matrix[i][j] << endl;
                    count--;
                }
            }
        }
        cout << endl << "the final matrix is^" << endl;

        //выводим итоговую матрицу в консоль

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                printf("%d%s", matrix[i][j], " ");

            }
            cout << endl;
        }
        cout << endl;
        //<задание 2>
        //отправка строк по процессам

        for (int i = 1; i < size; i++) {
            for (int j = i - 1; j < ROW; j += size - 1) {
                cout << "send row " << j << ": ";
                for (int k = 0; k < COL; k++) {
                    cout << matrix[j][k] << " ";
                }
                cout << "to process " << i << endl;
                MPI_Send(matrix[j], COL, MPI_INT, i, 1,
                         MPI_COMM_WORLD);

            }

        }
        cout << endl;

        //получение произвелений от процессов и вычисление общего произведения
        int received_result;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&received_result, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
            cout << "received result from process " << i << " is " << received_result << endl;
            result *= received_result;
        }
        cout << endl;
        cout << "total result is " << result << endl;
        //</задание 2>


        //получаем строку и считаем баланс
    } else {

        int *buf = new int[COL];

        for (int i = rank - 1; i < ROW; i += size - 1) {
            MPI_Recv(buf, COL, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);


            for (int j = 0; j < COL; j++) {

                balance += (buf[j] / abs(buf[j]));

            }

        }
        MPI_Send(&balance, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        //<задание 2>

        for (int i = rank - 1; i < ROW; i += size - 1) {
            MPI_Recv(buf, COL, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);


            for (int j = 0; j < COL; j++) {

                if (buf[j] >= -3 && buf[j] <= 3 && buf[j] != 0) {
                    result *= buf[j];
                }

            }

        }
        MPI_Send(&result, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        //</задание 2>
    }


    //<задание 2 лаб 4.2>
    //расчет числа элементов для отправки в каждый процесс

    int count;
    int result_proc = 1;

    if ((COL * ROW) % size){
        count = COL * ROW / size + 1;
    } else{
        count = COL * ROW / size;
    }
    int *rbuf = new int[count];
   //рассылка по частям и вычисление прозведения, в 0 тоже посылается
    MPI_Scatter(matrix, count, MPI_INT, rbuf, count, MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < count; i++) {
            if (rbuf[i] >= -3 && rbuf[i] <= 3 && rbuf[i] != 0) {
                result_proc *= rbuf[i];
            }
        }

    //получение ответа от процессов и перемножение
    MPI_Reduce(&result_proc, &result, 1,  MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
    if (rank == 0){
        cout << "total result for lab 4.2 is " << result << endl;
    }



    //</задание 2 лаб 4.2>
    MPI_Finalize();
    return 0;
}