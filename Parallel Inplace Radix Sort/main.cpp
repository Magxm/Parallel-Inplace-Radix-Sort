// Parallel Inplace Radix Sort.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "PIRS.hpp"
#include <random>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

typedef uint8_t SortType;

PIRS::PIRSorter<SortType, PIRS::ByteAdapter, 8> PIRSorter;
void PIRSSort(SortType* data, size_t dataSize)
{
	PIRSorter.Sort(data, dataSize);
}

void swap(SortType* xp, SortType* yp)
{
	SortType temp = *xp;
	*xp = *yp;
	*yp = temp;
}

const size_t minPartitionSize = 1000000;

template <class ForwardIterator,
	typename Compare = std::less<
	typename std::iterator_traits<ForwardIterator>::value_type
	>
>
void quicksortMT(ForwardIterator first, ForwardIterator last, Compare comp = Compare())
{
	using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
	using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
	difference_type dist = std::distance(first, last);
	if (dist < 2)
	{
		return;
	}
	else
	{
		//Multithread
		auto pivot = *std::next(first, dist / 2);
		auto     ucomp = [pivot, &comp](const value_type& em) { return  comp(em, pivot); };
		auto not_ucomp = [pivot, &comp](const value_type& em) { return !comp(pivot, em); };

		auto middle1 = std::partition(first, last, ucomp);
		auto middle2 = std::partition(middle1, last, not_ucomp);

		if (dist < minPartitionSize)
		{
			//No Multithread
			quicksortMT(first, middle1, comp);
			quicksortMT(middle2, last, comp);
		}
		else
		{
			//Multithread
			auto f1 = std::async(std::launch::async, [first, middle1, &comp] { quicksortMT(first, middle1, comp); });
			auto f2 = std::async(std::launch::async, [middle2, last, &comp] { quicksortMT(middle2, last, comp); });
			f1.wait();
			f2.wait();
		}
	}
}

void QuicksortMT(SortType* data, size_t dataSize)
{
	quicksortMT(data, data + dataSize);
}

void SelectionSort(SortType* data, size_t dataSize)
{
	int i, j;
	SortType temp;
	for (i = 0; i < dataSize; i++)
		for (j = i + 1; j < dataSize; j++)
		{
			if (data[i] > data[j])
			{
				temp = data[i];
				data[i] = data[j];
				data[j] = temp;
			}
		}
}


SortType findMax(SortType* arr, size_t size)
{
	SortType max = arr[0];
	for (size_t i = 1; i < size; i++)
		max = (arr[i] > max) ? arr[i] : max;
	return max;
}

void radixSort(SortType* arr, size_t size)
{
	SortType max = findMax(arr, size);
	std::queue<SortType> bucket[10];
	for (size_t n = 1; n <= max; n *= 10)
	{
		for (size_t i = 0; i < size; i++)
			bucket[(arr[i] / n) % 10].push(arr[i]);
		size_t k = 0;
		for (size_t j = 0; j < 10; j++)
		{
			while (!bucket[j].empty())
			{
				arr[k++] = bucket[j].front();
				bucket[j].pop();
			}
		}
	}
}


void BubbleSort(SortType* data, size_t dataSize)
{
	int i, j;
	bool swapped;
	for (i = 0; i < dataSize - 1; i++)
	{
		swapped = false;
		for (j = 0; j < dataSize - i - 1; j++)
		{
			if (data[j] > data[j + 1])
			{
				swap(&data[j], &data[j + 1]);
				swapped = true;
			}
		}
		if (swapped == false)
			break;
	}
}

void InsertionSort(SortType* data, size_t dataSize)
{
	// Function to do insertion sort.
	int i, j;
	SortType key;
	for (i = 1; i < dataSize; i++)
	{
		key = data[i];
		j = i - 1;
		while (j >= 0 && data[j] > key)
		{
			data[j + 1] = data[j];
			j = j - 1;
		}
		data[j + 1] = key;
	}
}

void join(SortType* arr, SortType left, SortType mid, SortType right)
{
	auto const subArrayOne = mid - left + 1;
	auto const subArrayTwo = right - mid;

	// Create temp arrays
	auto* leftArray = new SortType[subArrayOne],
		* rightArray = new SortType[subArrayTwo];

	// Copy data to temp arrays leftArray[] and rightArray[]
	for (auto i = 0; i < subArrayOne; i++)
		leftArray[i] = arr[left + i];
	for (auto j = 0; j < subArrayTwo; j++)
		rightArray[j] = arr[mid + 1 + j];

	auto indexOfSubArrayOne = 0, // Initial index of first sub-array
		indexOfSubArrayTwo = 0; // Initial index of second sub-array
	int indexOfMergedArray = left; // Initial index of merged array

	// Merge the temp arrays back into array[left..right]
	while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo)
	{
		if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
		{
			arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
			indexOfSubArrayOne++;
		}
		else
		{
			arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
			indexOfSubArrayTwo++;
		}
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// left[], if there are any
	while (indexOfSubArrayOne < subArrayOne)
	{
		arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
		indexOfSubArrayOne++;
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// right[], if there are any
	while (indexOfSubArrayTwo < subArrayTwo)
	{
		arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
		indexOfSubArrayTwo++;
		indexOfMergedArray++;
	}
}

void mergeSort(SortType* arr, size_t l, size_t r)
{
	// The major implementation of merge sort algorithm
	if (l < r)
	{
		size_t m = l + (r - l) / (size_t)2;
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);
		join(arr, l, m, r);
	}
}

void MergeSort(SortType* data, size_t dataSize)
{
	mergeSort(data, 0, dataSize - 1);
}

int cmp(const void* a, const void* b)
{
	return (*(SortType*)a - *(SortType*)b);
}

void QuickSort(SortType* data, size_t dataSize)
{
	std::qsort(data, dataSize, sizeof(SortType), cmp);
}

void IntroSort(SortType* data, size_t dataSize)
{
	std::sort(data, data + dataSize);
}

void RadixSort(SortType* data, size_t dataSize)
{
	radixSort(data, dataSize);
}

struct BenchmarkEntry
{
	SortType* List;
	size_t ListSize;
};

typedef void (*SortingAlgo)(SortType* data, size_t dataSize);
double BenchmarkAlgorithm(SortingAlgo sortFunction, std::wstring sortingAlgoName, const BenchmarkEntry* benckmarkEntry)
{
	std::wofstream benchmarkFile;
	benchmarkFile.open(sortingAlgoName + L".csv", std::ios_base::app);

	std::wcout << L"Testing " << sortingAlgoName << L" with list of size " << benckmarkEntry->ListSize << std::endl;
	std::wcout << L"Creating copy of sorting list..." << std::endl;
	SortType* toSortData = new SortType[benckmarkEntry->ListSize];
	memcpy(toSortData, benckmarkEntry->List, benckmarkEntry->ListSize * sizeof(SortType));
	std::wcout << L"Starting sorting algorithm..." << std::endl;
	auto start = std::chrono::system_clock::now();
	sortFunction(toSortData, benckmarkEntry->ListSize);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedSecondsDuration = end - start;
	double elapsedSeconds = elapsedSecondsDuration.count();
	std::wcout << L"Sorting algorithm finished in " << elapsedSeconds << L" seconds!" << std::endl;
	benchmarkFile << std::fixed << std::setprecision(10) << benckmarkEntry->ListSize << L"," << elapsedSeconds << "\n";
	benchmarkFile << std::flush;
	std::wcout << L"Cleaning up..." << std::endl;
	delete[] toSortData;

	benchmarkFile.close();

	return elapsedSeconds;
}

BenchmarkEntry* CreateBenchmarkEntry(size_t size)
{
	std::wcout << L"Creating benchmarks for list with " << size << L" entries..." << std::endl;

	SortType* list = new SortType[size];
	std::random_device rd;
	std::mt19937 gen(rd());
#undef min
#undef max
	std::uniform_int_distribution<> distribution(std::numeric_limits<SortType>::min(), std::numeric_limits<SortType>::max());

	for (size_t i = 0; i < size; ++i)
	{
		list[i] = distribution(gen);
	}

	std::shuffle(list, list + size, gen);

	return new BenchmarkEntry{ list, size };
}

double lastPIRSTime = 0;
double lastSelectionTime = 0;
double lastBubbleTime = 0;
double lastInsertionTime = 0;
double lastMergeTime = 0;
double lastQSTime = 0;
double lastIntroTime = 0;
double lastRadixTime = 0;
double lastQSMTTime = 0;

double secondsThreshhold = 300;
bool Benchmark(const BenchmarkEntry* benckmarkEntry)
{
	bool sortingAlgoLeft = false;

	/*
	if (lastMergeTime <= secondsThreshhold)
	{
		lastMergeTime = BenchmarkAlgorithm(MergeSort, L"MergeSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}
	*/

	if (lastPIRSTime <= secondsThreshhold)
	{
		lastPIRSTime = BenchmarkAlgorithm(PIRSSort, L"Parallel Inplace Radix Sort", benckmarkEntry);
		sortingAlgoLeft = true;
	}

	if (lastQSMTTime <= secondsThreshhold)
	{
		lastQSMTTime = BenchmarkAlgorithm(QuicksortMT, L"ParallelQuicksort", benckmarkEntry);
		sortingAlgoLeft = true;
	}

	/*
	if (lastSelectionTime <= secondsThreshhold)
	{
		lastSelectionTime = BenchmarkAlgorithm(SelectionSort, L"SelectionSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}
	if (lastBubbleTime <= secondsThreshhold)
	{
		lastBubbleTime = BenchmarkAlgorithm(BubbleSort, L"BubbleSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}

	if (lastInsertionTime <= secondsThreshhold)
	{
		lastInsertionTime = BenchmarkAlgorithm(InsertionSort, L"InsertionSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}*/

	if (lastIntroTime <= secondsThreshhold)
	{
		lastIntroTime = BenchmarkAlgorithm(IntroSort, L"IntroSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}

	if (lastQSTime <= secondsThreshhold)
	{
		lastQSTime = BenchmarkAlgorithm(QuickSort, L"QuickSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}


	if (lastRadixTime <= secondsThreshhold)
	{
		lastRadixTime = BenchmarkAlgorithm(RadixSort, L"RadixSort", benckmarkEntry);
		sortingAlgoLeft = true;
	}

	/*
	sortingAlgoLeft = true;
	BenchmarkAlgorithm(QuicksortMT, L"ParallelQuicksort", benckmarkEntry);
	BenchmarkAlgorithm(QuickSort, L"QuickSort", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_64, L"PIRSSort_64", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_128, L"PIRSSort_128", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_1024, L"PIRSSort_1024", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_8192, L"PIRSSort_8192", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_32k, L"PIRSSort_32k", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_65k, L"PIRSSort_64k", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_130k, L"PIRSSort_130k", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_500k, L"PIRSSort_500k", benckmarkEntry);
	BenchmarkAlgorithm(PIRSSort_1kk, L"PIRSSort_1kk", benckmarkEntry);#
	*/
	return sortingAlgoLeft;
}

int main()
{
	std::vector<size_t> benchmarkSizes;
	size_t value = 1;
	while (value < std::numeric_limits<size_t>::max() / 2)
	{
		benchmarkSizes.emplace_back(value);
		value += 1;
		value *= 1.05;
	}

	for (auto size : benchmarkSizes)
	{
		auto benchmarkEntry = CreateBenchmarkEntry(size);
		bool done = !Benchmark(benchmarkEntry);
		delete[] benchmarkEntry->List;
		delete benchmarkEntry;
		if (done) break;
	}
}