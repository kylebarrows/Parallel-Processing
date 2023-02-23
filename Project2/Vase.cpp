#include <iostream>
#include <thread>
#include <random>
#include <algorithm>
#include <chrono>
#include <mutex>

const int NUM_THREADS = 500;
std::mutex mutex;

bool available = true; 
bool hasSeenVase[NUM_THREADS] = { false };
int satisfiedGuests = 0;

void lookAtVase(int threadIndex)
{
	while (satisfiedGuests < NUM_THREADS)
	{
		mutex.lock();

		if (available && !hasSeenVase[threadIndex])
		{
			available = false;
			std::cout << threadIndex << " has entered the room" << std::endl;
			hasSeenVase[threadIndex] = true;
			satisfiedGuests++;
			available = true;
		}

		mutex.unlock();
	}
}

int main()
{
	std::thread threads[NUM_THREADS];

	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i] = std::thread(lookAtVase, i);
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i].join();
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> runTime = endTime - startTime;

	std::cout << "Done in " << runTime.count() << "ms" << std::endl;

	return 0;
}