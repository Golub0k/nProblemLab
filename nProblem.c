#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

static double delta_time = 0.05; // Шаг времени
static int thread_count;         // Количество потоков
int bodies, timeSteps;           // Количество частиц, количество временных шагов
//static pthread_cond_t cv;
//static pthread_mutex_t mutex;
//int cntr = 0;

//---------Cтруктура vector представляет двумерный вектор с координатами x и y ------------------------------------
typedef struct
{
    double x, y;
} vector;
//-----------------------------------------------------------------------------------------------------------------

//---------Cтруктура thread_info содержит начальный и конечный индексы частиц, которые поток должен обработать-----
typedef struct
{
    int start;
    int end;
} thread_info;
//-----------------------------------------------------------------------------------------------------------------

double *masses, GravConstant;                   // Массив масс частиц, константа гравитации
vector *positions, *velocities, *accelerations; // Массивы позиций, скоростей и ускорений частиц

//---------Функция сложения векторов-------------------------------------------------------------------------------
vector addVectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};
    return c;
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция умножения векторов------------------------------------------------------------------------------
vector scaleVector(double b, vector a)
{
    vector c = {b * a.x, b * a.y};
    return c;
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция вычитания векторов------------------------------------------------------------------------------
vector subtractVectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};
    return c;
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция вычисления модуля (длины) вектора---------------------------------------------------------------
double mod(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция для инициализации данных из файла---------------------------------------------------------------
void initiateSystem(char *fileName)
{
    FILE *fp = fopen(fileName, "r");
    fscanf(fp, "%lf%d%d", &GravConstant, &bodies, &timeSteps);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));

    for (int i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция для разрешения коллизий (столкновений частиц)---------------------------------------------------
void resolveCollisions()
{
    // Проверка столкновения частиц и изменение их скорости при необходимости (одинаковые позиции == столкновение)
    int i, j;
    for (i = 0; i < bodies - 1; i++)
        for (j = i + 1; j < bodies; j++)
        {
            if (fabs(positions[i].x - positions[j].x) < 0.000001 && fabs(positions[i].y - positions[j].y) < 0.000001)
            {
                vector temp = velocities[i];
                velocities[i] = velocities[j];
                velocities[j] = temp;
            }
        }
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция для вычисления ускорений частиц в указанном диапазоне, исходя из закона гравитации--------------
void computeAccelerations(int start, int end)
{
    for (int i = start; i < end; i++)
    {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
        for (int j = 0; j < bodies; j++)
        {
            if (i != j)
            {
                double d = mod(subtractVectors(positions[i], positions[j]));
                accelerations[i] = addVectors(accelerations[i], scaleVector(GravConstant * masses[j] / (d * d * d), subtractVectors(positions[j], positions[i])));
            }
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция для обновления скоростей частиц, исходя из рассчитанных ускорений и заданного шага времени------
void computeVelocities(int start, int end)
{
    for (int i = start; i < end; i++)
        velocities[i] = addVectors(velocities[i], scaleVector(delta_time, accelerations[i]));
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция для обновления позиций частиц, исходя из рассчитанных скоростей и заданного шага времени--------
void computePositions(int start, int end)
{
    for (int i = start; i < end; i++)
        positions[i] = addVectors(positions[i], scaleVector(delta_time, velocities[i]));
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция, выполняемая в каждом потоке для симуляции движения частиц--------------------------------------
void *simulate(void *arg)
{
    thread_info *a = (thread_info *)arg;
    computeAccelerations(a->start, a->end); // Вычисление ускорений

    //pthread_mutex_lock(&mutex); // Синхронизация потоков
    //cntr++;
    //while (cntr != thread_count) {
    //    pthread_cond_wait(&cv, &mutex);
    //}
    //pthread_cond_broadcast(&cv);
    //pthread_mutex_unlock(&mutex);

    computePositions(a->start, a->end); // Обновление позиций частиц
    computeVelocities(a->start, a->end); // Обновление скоростей частиц
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------

//---------Функция, выполняемая в каждом потоке для симуляции движения частиц--------------------------------------
int main(int argC, char *argV[])
{
    if (argC != 4)
    {
        printf("Usage: %s <input_filename> <output_filename> <thread_count>", argV[0]);
        return -1;
    }
    initiateSystem(argV[1]);
    char *output_filename = argV[2];
    thread_count = atoi(argV[3]);
    pthread_t threads[thread_count];
    thread_info thread_info[thread_count];
    //pthread_cond_init(&cv, NULL);
    //pthread_mutex_init(&mutex, NULL);

    int chunksNum = bodies / thread_count;
    for (int i = 0; i < thread_count; i++)
    {
        thread_info[i].start = chunksNum * i;
        thread_info[i].end = chunksNum * (i + 1);
    }
    thread_info[thread_count - 1].end = bodies;

    FILE *fpt;
    fpt = fopen(output_filename, "w+");

    fprintf(fpt, "t, ");
    for (int i = 0; i < bodies; i++)
        fprintf(fpt, "x%d, y%d, ", i + 1, i + 1);
    fprintf(fpt, "\n");

    for (int i = 0; i < timeSteps; i++)
    {
        for (int j = 0; j < thread_count; ++j)
            pthread_create(&threads[j], NULL, simulate, &thread_info[j]);

        for (int j = 0; j < thread_count; ++j)
            pthread_join(threads[j], NULL);

        //cntr = 0;
        resolveCollisions();

        fprintf(fpt, "%d, ", i + 1);
        for (int j = 0; j < bodies; j++)
            fprintf(fpt, "%lf, %lf, ", positions[j].x, positions[j].y);
        fprintf(fpt, "\n");
    }

    fclose(fpt);
    //pthread_cond_destroy(&cv);
    //pthread_mutex_destroy(&mutex);
    free(masses);
    free(positions);
    free(velocities);
    free(accelerations);
    return 0;
}
//-----------------------------------------------------------------------------------------------------------------