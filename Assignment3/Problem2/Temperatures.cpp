#include <iostream>
#include <thread>
#include <random>
#include <atomic>
#include <algorithm>
#include <chrono>

using namespace std;
atomic<int> minutes(0);
atomic<bool> finished(false);
vector<vector<int>> reads(60, vector<int>(8));

int GetReading()
{
	std::random_device s;
	std::mt19937 rng(s());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 170);
	return dist6(rng);
}

void CollectData(int id)
{
	while (!finished.load())
	{
		int minute = minutes.load();

		int reading = GetReading() - 100;
		reads[minute][id] = reading;

		while (minutes.load() == minute && !finished.load()) {};
	}
}

void CollectReadings(vector<vector<int>> readings)
{
	int minimum[60];
	int maximum[60];

	for (int i = 0; i < 60; i++)
	{
		maximum[i] = *max_element(begin(reads[i]), end(reads[i]));
		minimum[i] = *min_element(begin(reads[i]), end(reads[i]));
	}

	int startTime = 0;
	int maxDiff = 0;
	for (int i = 0; i < 50; i++)
	{
		int max = *max_element(begin(maximum) + i, begin(maximum) + 10 + i);
		int min = *min_element(begin(minimum) + i, begin(minimum) + 10 + i);

		int diff = max - min;
		if (diff > maxDiff)
		{
			maxDiff = diff;
			startTime = i;
		}
	}

	sort(begin(maximum), end(maximum));
	sort(begin(minimum), end(minimum));

	cout << "Top 5 highest: " << maximum[55] << " " << maximum[56] << " " << maximum[57] << " " << maximum[58] << " " << maximum[59] << "\n";
	cout << "Top 5 lowest: " << minimum[0] << " " << minimum[1] << " " << minimum[2] << " " << minimum[3] << " " << minimum[4] << "\n";

	cout << "Largest temperature difference of " << maxDiff << " recorded between " << startTime << " to " << startTime + 10 << endl;
}

int main()
{
	int hoursToRun = 50;
	std::thread threads[8];

	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 8; i++)
	{
		threads[i] = thread(CollectData, i);
	}

	for (int i = 0; i < hoursToRun; i++)
	{
		while (minutes.load() < 60)
		{
			this_thread::sleep_for(chrono::milliseconds(15));
			if (minutes.load() == 59)
			{
				break;
			}

			minutes.store(minutes.load() + 1);
		}

		cout << "Hour " << i << " has passed\n";
		CollectReadings(reads);
		minutes.store(0);
	}

	finished.store(true);

	for (int i = 0; i < 8; i++)
	{
		threads[i].join();
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> runTime = endTime - startTime;

	std::cout << "Done in " << runTime.count() << "ms" << std::endl;

	return 0;
}
