#include <iostream>
#include <random>
#include <thread>//библиотека для мультипоточности
#include <chrono>//библиотека для таймера
#include <mutex>//библиотека, чтобы блокировать

using namespace std;

mutex g_lock;//объект, чтобы заблокировать доступ к чему-то для всех, кроме одного потока

			 //сортировки
void quicksort(int *array, int left, int right)
{
	int i = left;
	int j = right;
	int pivot = array[(i + j) / 2];
	int temp;
	while (i <= j)
	{
		while (array[i] < pivot)
			i++;
		while (array[j] > pivot)
			j--;
		if (i <= j)
		{
			temp = array[i];
			array[i] = array[j];
			array[j] = temp;
			i++;
			j--;
		}
	}
	if (j > left)
		quicksort(array, left, j);
	if (i < right)
		quicksort(array, i, right);
}

void bubblesort(int *array, int size)
{
	int tmp;
	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = 0; j < size - 1; ++j)
		{
			if (array[j + 1] < array[j])
			{
				tmp = array[j + 1];
				array[j + 1] = array[j];
				array[j] = tmp;
			}
		}
	}
}

/*вывод таймера в консоль. Общий для того, чтобы два потока не полезли записывать в консоль одновременно
Поэтому блокируется через созданный в начале объект*/
void showtime(chrono::duration<float> time, const char* namesort)
{
	g_lock.lock();
	cout << "Duration of the " << namesort << ": " << time.count() << endl;
	g_lock.unlock();
}

//таймеры сортировок
void quicksort_time(int *array, int left, int right)
{
	auto start = chrono::high_resolution_clock::now();
	quicksort(array, left, right);
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = end - start;
	showtime(duration, "Quick Sort");
}

void bubblesort_time(int *array, int size)
{
	auto start = chrono::high_resolution_clock::now();
	bubblesort(array, size);
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = end - start;
	showtime(duration, "Bubble Sort");
}

int main()
{
	//тут создается рандомный генератор для заполнения массивов
	default_random_engine generator;
	uniform_int_distribution<int> distribution(1, 1000);
	int size;
	cout << "Enter the size of the arrays:\n";
	cin >> size;
	//засекается время для выполнения программы после ввода размера массивов
	auto start = chrono::high_resolution_clock::now();
	int *array1 = new int[size];
	int *array2 = new int[size];
	//засекается время для рандомайзера, можно убрать
	auto start_rnd = chrono::high_resolution_clock::now();
	for (int i = 0; i < size; i++)
	{
		array1[i] = distribution(generator);
		array2[i] = array1[i];
	}
	auto end_rnd = chrono::high_resolution_clock::now();
	chrono::duration<float> duration_rnd = end_rnd - start_rnd;
	cout << "Duration of the Randomizer: " << duration_rnd.count() << endl;
	//создаются потоки для сортировок
	thread quick_thread(quicksort_time, array1, 0, (size - 1));
	thread bubble_thread(bubblesort_time, array2, size);
	//присоединяем потоки, чтобы они не уплыли, когда основная функция закончится
	if (quick_thread.joinable())
		quick_thread.join();
	if (bubble_thread.joinable())
		bubble_thread.join();
	delete[] array1;
	delete[] array2;
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = end - start;
	cout << "Duration of the Main Function: " << duration.count() << endl;
	system("pause");
	return 0;
}