#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <algorithm>
#include <chrono>
#include <mutex>

const int TARGET = 100000000;
const int NUM_THREADS = 8;
std::vector<int> primesGlobal;
std::mutex primesMutex;

void segmentedSieve(int index, int start, int end)
{
	//std::cout << "Thread " << index << " joined!" << std::endl;

	int limit = sqrt(end);

	// Using chars is faster than bools!
	std::vector<char> mark(limit + 1, false);
	std::vector<int> primes = {2};

	// Use the sieve of Eratosthenes
	for (int i = 3; i <= limit; i += 2)
	{
		if (!mark[i])
		{
			primes.emplace_back(i);
			for (int j = i * i; j <= limit; j += i)
			{
				mark[j] = true;
			}
		}
	}

	std::vector<char> isPrime(end - start + 1, true);

	// Use the primes found in the simple sieve to find the primes in range
	for (int i : primes)
	{
		int bound = std::max(i * i, (start + i - 1) / i * i);
		for (int j = bound; j <= end; j += i)
		{
			isPrime[j - start] = false;
		}
	}
	if (start == 1)
		isPrime[0] = false;
	
	primesMutex.lock();
	for (int i = 0; i < isPrime.size(); i++)
	{
		if (isPrime[i] == true)
			primesGlobal.push_back(i + start);
	}
	primesMutex.unlock();
}

// Returns the sum of primes in global array
long long getSumOfPrimes()
{
	long long sum = 0;
	for (int i = 0; i < primesGlobal.size(); i++)
	{
		sum += primesGlobal[i];
	}

	return sum;
}

// Returns a vector containing the N highest numbers in primes array
std::vector<int> getHighestNumbers(int n)
{
	std::sort(primesGlobal.begin(), primesGlobal.end());

	std::vector<int> highest(primesGlobal.end() - n, primesGlobal.end());
	return highest;
}

// Outputs result to file
void writeResultsToFile(std::chrono::duration<double, std::milli> ms)
{
	std::ofstream file("Results.txt");

	file << ms.count() / 1000 << " seconds. " << "\n";
	file << primesGlobal.size() << " primes found. " << "\n";

	long long sum = getSumOfPrimes();
	file << "sum of primes =  " << sum << "\n";

	std::vector<int> p = getHighestNumbers(10);
	for (int i = 0; i < p.size(); i++)
	{
		file << p[i] << " ";
	}

	file.close();
}

int main()
{
	std::vector<std::thread> threads;
	
	int range = TARGET / NUM_THREADS;

	auto startTime = std::chrono::high_resolution_clock::now();

#if true
	for (int i = 0; i < NUM_THREADS; i++)
	{
		int start = i * range + 1;
		int end = (i + 1) * range;
		threads.push_back(std::thread(segmentedSieve, i, start, end));
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i].join();
	}
#endif

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> timeMilli = endTime - startTime;

	writeResultsToFile(timeMilli);

	return 0;
}