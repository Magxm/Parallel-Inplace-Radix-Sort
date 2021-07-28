#pragma once

#include <thread>
#include <future>
#include <mutex>
#include <Windows.h>

namespace PIRS
{
	class ByteAdapter
	{
	public:
		inline static bool Evaluate(const int8_t& value, const size_t& step)
		{
			if (step == 0)
			{
				return (value & (1 << (7 - step))) == 0;
			}
			else
			{
				return (value & (1 << (7 - step))) != 0;
			}
		};
	};

	class UByteAdapter
	{
	public:
		inline static bool Evaluate(const uint8_t& value, const size_t& step)
		{
			return (value & (1 << (7 - step))) != 0;
		};
	};

	class IntAdapter
	{
	public:
		inline static bool Evaluate(const int& value, const size_t& step)
		{
			if (step == 0)
			{
				return (value & (1 << (31 - step))) == 0;
			}
			else
			{
				return (value & (1 << (31 - step))) != 0;
			}
		};
	};

	class UIntAdapter
	{
	public:
		inline static bool Evaluate(const unsigned int& value, const size_t& step)
		{
			return (value & (1 << (31 - step))) != 0;
		};
	};

	template<typename ToSortType, typename Adapter, size_t MAX_STEPS, size_t NEW_THREAD_THRESHOLD = 8192>
	class PIRSorter
	{
	public:
		std::vector<std::future<void>> _Futures;
		std::mutex _FuturesManipMutex;
		void _InternalSort(ToSortType* data, size_t elementCount, size_t step)
		{
			if (elementCount <= 1)
			{
				return;
			};

			size_t i = 0;
			ToSortType tmp;
			bool swapped;
			size_t j = elementCount - 1;
			while (i < elementCount)
			{
				if (Adapter::Evaluate(data[i], step))
				{
					swapped = false;
					while(j > i)
					{
						if (!Adapter::Evaluate(data[j], step))
						{
							tmp = data[i];
							data[i] = data[j];
							data[j] = tmp;
							swapped = true;
							break;
						}
						j--;
					}

					if (!swapped)
					{
						//Step done
						break;
					}
				}
				++i;
			}

			if (step + 1 != MAX_STEPS)
			{
				//not done
				if (i == elementCount || i == 0)
				{
					_FuturesManipMutex.lock();
					//All elements are evaluator true or false
					_Futures.emplace_back(std::async(&PIRSorter::_InternalSort, this, data, elementCount, step + 1));
					_FuturesManipMutex.unlock();
				}
				else
				{
					if (i < NEW_THREAD_THRESHOLD || elementCount - i < NEW_THREAD_THRESHOLD)
					{
						/*
						const static auto SortBothInOneThread = [](PIRSorter* _this, ToSortType* data, size_t elementCount, size_t step, size_t i)
						{
							if (i > 1)
							{
								_this->_InternalSort(data, i, step);
							}
							if (elementCount - i > 1)
							{
								_this->_InternalSort(data + i, elementCount - i, step);
							}
						};
						//No two threads
						if (i > 1 || elementCount - i > 1)
						{
						    _FuturesManipMutex.lock();
						    _Futures.emplace_back(std::async(SortBothInOneThread, this, data, elementCount, step + 1, i));
							_FuturesManipMutex.unlock();
						}
						*/
						_InternalSort(data, i, step + 1);
						_InternalSort(data + i, elementCount - i, step + 1);
					}
					else
					{
						//New thread
						if (i > 0 || elementCount - i > 0)
						{
							_FuturesManipMutex.lock();
							if (i > 0)
							{
								_Futures.emplace_back(std::async(&PIRSorter::_InternalSort, this, data, i, step + 1));
							}
							if (elementCount - i > 0)
							{
								_Futures.emplace_back(std::async(&PIRSorter::_InternalSort, this, data + i, elementCount - i, step + 1));
							}
							_FuturesManipMutex.unlock();
						}
						
					}
				}
			}
			//Done!
		}
	public:
		PIRSorter()
		{
			_Futures = std::vector<std::future<void>>();
			_Futures.reserve(2048);
		};

		void Sort(ToSortType* data, size_t elementCount)
		{
			_Futures.clear();
			_InternalSort(data, elementCount, 0);

			size_t i = 0;
			_FuturesManipMutex.lock();
			size_t futuresSize = _Futures.size();
			_FuturesManipMutex.unlock();
			while (i < futuresSize)
			{
				_FuturesManipMutex.lock();
				futuresSize = _Futures.size();
				_FuturesManipMutex.unlock();
				_Futures[i].wait();
				i++;
			}

			_Futures.clear();
		}
	};
}