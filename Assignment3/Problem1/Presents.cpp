#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include <algorithm>
#include <atomic>
#include <chrono>
#include "ConcurrentList.h"

using namespace std;

int DecideAction()
{
	std::random_device s;
	std::mt19937 rng(s());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 100);
	return dist6(rng);
}

void Servant(vector<int>& bag, atomic_int& inBag, atomic_int& written, ConcurrentList* list)
{
	while (inBag.load() < bag.size() || written.load() < bag.size())
	{
		int decision = DecideAction();
		if (decision <= 45)
		{
			try
			{
				if (inBag.load() < bag.size())
				{
					int toAdd = bag[inBag.fetch_add(1)];
					list->add(toAdd);
				}
			}
			catch (...) {}
		}
		else if (decision <= 91)
		{
			try
			{
				if (written.load() < bag.size())
				{
					int cardToWrite = bag[written.fetch_add(1)];
					list->remove(cardToWrite);
				}
			}
			catch (...) {}
		}
		else
		{
			int toCheck = bag[inBag.load()];

			list->contains(toCheck);
		}
	}
}


int main()
{
	const int numServants = 4;
	int totalPresents = 500000;
	std::thread threads[numServants];

	vector<int> bag(totalPresents);

	for (int i = 0; i < totalPresents; i++)
	{
		bag[i] = i;
	}

	random_device s;
	mt19937 generator(s());
	shuffle(begin(bag), end(bag), generator);

	atomic_int inBag = 0;
	atomic_int written = 0;
	ConcurrentList* list = new ConcurrentList();
	list->add(100);

	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < numServants; i++)
	{
		threads[i] = thread(Servant, ref(bag), ref(inBag), ref(written), list);
	}

	for (int i = 0; i < numServants; i++)
	{
		threads[i].join();
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> runTime = endTime - startTime;

	std::cout << "Done in " << runTime.count() << "ms" << std::endl;

	return 0;
}