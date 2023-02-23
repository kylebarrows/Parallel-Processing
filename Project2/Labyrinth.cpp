#include <iostream>
#include <thread>
#include <random>
#include <algorithm>
#include <chrono>
#include <mutex>

const int NUM_THREADS = 50;
std::mutex mutex;
bool chosenGuests[NUM_THREADS];
bool hasCupcake = true;
int count = 0;
int guestInLabyrinth;

int chooseGuestToEnter(int min, int max)
{
	std::random_device s;
	std::mt19937 rng(s());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);
	return dist6(rng);
}

void checkCupcake()
{
	while (count < NUM_THREADS)
	{
		mutex.lock();

		if (guestInLabyrinth == 0)
		{
			// Eat the cupcake if it's there
			if (hasCupcake)
			{
				count++;
				hasCupcake = false;
			}

			chosenGuests[0] = true;
		}

		mutex.unlock();
	}
}

void enterLabyrinth(int threadIndex)
{
	while (count < NUM_THREADS)
	{
		mutex.lock();

		if (guestInLabyrinth == threadIndex && !hasCupcake && !chosenGuests[threadIndex])
		{
			std::cout << threadIndex << " requested a cupcake" << std::endl;
			hasCupcake = true;
			chosenGuests[threadIndex] = true;
		}

		mutex.unlock();
	}
}


int main()
{
	std::thread threads[NUM_THREADS];

	auto startTime = std::chrono::high_resolution_clock::now();

	threads[0] = std::thread(checkCupcake);
	for (int i = 1; i < NUM_THREADS; i++)
	{
		threads[i] = std::thread(enterLabyrinth, i);
	}

	while (count < NUM_THREADS)
	{
		guestInLabyrinth = chooseGuestToEnter(0, NUM_THREADS);
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i].join();
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> runTime = endTime - startTime;

	std::cout << "Done in " << runTime.count() << "ms" << std::endl;

	char temp;
	std::cin >> temp;

	return 0;
}