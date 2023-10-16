#pragma once
#include <iostream>
#include <exception>
#include <list>
#include <condition_variable>
#include <mutex>
#include <thread>
#define ThrowOutOfRange throw exception("Index is out of range.");
using namespace std;

template <typename T>
class DynamicArrayBlock
{
public:
	DynamicArrayBlock(int length)
	{
		this->length = length;
		data = (T*)malloc(sizeof(T) * length);
	}
	~DynamicArrayBlock()
	{
		delete data;
		cout << "\nDynamicBlock 소멸 \n";
	}
	T& operator[](int index)
	{
		if (index < 0 || index >= length)
		{
			ThrowOutOfRange;
		}
		else
		{
			return data[index];
		}
	}
	int GetLength()
	{
		return length;
	}
private:
	int length;
	T* data;
};

template <typename T>
class DynamicArray
{
public:
	DynamicArray(int capacity) : capacity(capacity) {}
	~DynamicArray()
	{
		cout << "\nDynamicArray 소멸 \n";
	}
	int GetCount()
	{
		return insertIndex;
	}
	/// <summary>
	/// 최대한 Pop을 이용해주세요.
	/// </summary>
	/// <returns></returns>
	T PopByValue()
	{
		if (insertIndex <= 0)
		{
			ThrowOutOfRange;
		}
		else
		{
			insertIndex--;
			return GetRefOfElement(insertIndex);
		}
	}
	T& Pop()
	{
		if (insertIndex <= 0)
		{
			ThrowOutOfRange;
		}
		else
		{
			insertIndex--;
			return GetRefOfElement(insertIndex);
		}
	}
	void Clear()
	{
		insertIndex = 0;
	}
	template<typename... Args>
	void AddByArgs(const Args... args)
	{
		if (insertIndex >= GetTotalLength())
		{
			arrayBlocks.emplace_back(capacity);
		}
		GetRefOfElement(insertIndex) = T(args...);
		insertIndex++;
	}
	/// <summary>
	/// 총 두 번의 값복사가 일어납니다.
	/// 최대한 Add 함수를 이용해주세요.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		if (insertIndex >= GetTotalLength())
		{
			arrayBlocks.emplace_back(capacity);
		}
		GetRefOfElement(insertIndex) = element;
		insertIndex++;
	}
	/// <summary>
	/// 대입 연산자 정의에 따라 값복사 일어남.
	/// </summary>
	/// <param name="element"></param>
	void Add(const T& element)
	{
		if (insertIndex >= GetTotalLength())
		{
			arrayBlocks.emplace_back(capacity);
		}
		GetRefOfElement(insertIndex) = element;
		insertIndex++;
	}
	T& operator[](int index)
	{
		if (index < 0 || index >= insertIndex)
		{
			ThrowOutOfRange;
		}
		else
		{
			return GetRefOfElement(index);
		}
	}
	int GetCapacity()
	{
		return capacity;
	}
protected:
	int insertIndex = 0;
	int capacity;
	list<DynamicArrayBlock<T>> arrayBlocks;
	T& GetRefOfElement(int index)
	{
		int localIndex;
		DynamicArrayBlock<T>& block = GetBlockByGlobalIndex(arrayBlocks, index, &localIndex);
		return block[localIndex];
	}
	int GetTotalLength()
	{
		return capacity * arrayBlocks.size();
	}
	int GetLocalIndex(int globalIndex, int blockIndex)
	{
		return globalIndex - (capacity * blockIndex);
	}
	DynamicArrayBlock<T>& GetBlockByGlobalIndex(list<DynamicArrayBlock<T>>& arrayBlocks, int globalIndex, int* localIndex)
	{
		int blockIndex = globalIndex / capacity;
		*localIndex = GetLocalIndex(globalIndex, blockIndex);
		if (unsigned(blockIndex) < (arrayBlocks.size() / 2))
		{
			typename list<DynamicArrayBlock<T>>::iterator forwardIterator = arrayBlocks.begin();
			for (int i = 0; i < blockIndex; i++)
			{
				forwardIterator++;
			}
			return *forwardIterator;
		}
		else
		{
			typename list<DynamicArrayBlock<T>>::reverse_iterator backwardIterator = arrayBlocks.rbegin();
			int loopCount = arrayBlocks.size() - blockIndex - 1;
			for (int i = 0; i < loopCount; i++)
			{
				backwardIterator++;
			}
			return *backwardIterator;
		}
	}
};

template <typename T>
class StaticArray
{
public:
	StaticArray(T* originalArray, int arrayLength) : capacity(arrayLength), myArray(originalArray), insertIndex(arrayLength) {}
	StaticArray(int capacity, T* originalArray, int arrayLength) : capacity(capacity), myArray(originalArray), insertIndex(arrayLength) {}
	StaticArray(int capacity) : capacity(capacity)
	{
		myArray = new T[capacity];
	}
	~StaticArray()
	{
		delete myArray;
		cout << "\nStaticArray 소멸\n";
	}
	T& operator[](int index)
	{
		if (index < 0 || index >= insertIndex)
		{
			ThrowOutOfRange;
		}
		else
		{
			return myArray[index];
		}
	}
	/// <summary>
	/// 최대한 Pop을 이용해주세요.
	/// </summary>
	/// <returns></returns>
	T PopByValue()
	{
		if (insertIndex <= 0)
		{
			ThrowOutOfRange;
		}
		else
		{
			insertIndex--;
			return myArray[insertIndex];
		}
	}
	T& Pop()
	{
		if (insertIndex <= 0)
		{
			ThrowOutOfRange;
		}
		else
		{
			insertIndex--;
			return myArray[insertIndex];
		}
	}
	template<typename... Args>
	void AddByArgs(const Args... args)
	{
		if (insertIndex > capacity)
		{
			ThrowOutOfRange;
		}
		else
		{
			myArray[insertIndex] = T(args...);
			insertIndex++;
		}
	}
	/// <summary>
	/// 총 두 번의 값복사가 일어납니다.
	/// 최대한 Add 함수를 이용해주세요.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		if (insertIndex >= capacity)
		{
			ThrowOutOfRange;
		}
		myArray[insertIndex] = element;
		insertIndex++;
	}
	/// <summary>
	/// 대입 연산자 정의에 따라 값복사 일어남.
	/// </summary>
	/// <param name="element"></param>
	void Add(const T& element)
	{
		if (insertIndex >= capacity)
		{
			ThrowOutOfRange;
		}
		myArray[insertIndex] = element;
		insertIndex++;
	}
	void Clear()
	{
		insertIndex = 0;
	}
	int GetCount()
	{
		return insertIndex;
	}
	int GetCapacity()
	{
		return capacity;
	}
protected:
	int insertIndex = 0;
	int capacity;
	T* myArray;
};

class DynamicString : public DynamicArray<char>
{
public:
	DynamicString(int capacity) : DynamicArray<char>(capacity) {}
	~DynamicString()
	{
		cout << "\nDynamicString 소멸\n";
	}
	void Append(const char* string)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		for (size_t i = 0; string[i] != '\0'; i++)
		{
			this->Add(string[i]);
		}
		this->Add('\0');
	}
	void Append(const char& singleChar)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		this->Add(singleChar);
		this->Add('\0');
	}
	char& GetNewCharInHeap()
	{
		int count = this->GetCount();
		char* charArray = new char[count];
		for (int i = 0; i < count; i++)
		{
			charArray[i] = (*this)[i];
		}
		return *charArray;
	}
	void CopyChar(char* target, int length)
	{
		for (int i = 0; i < length; i++)
		{
			target[i] = (*this)[i];
		}
	}
};

class StaticString : public StaticArray<char>
{
public:
	//\0 때문에 혹시 실수할까봐 붙여준 상수 +1
	StaticString(const char* string, int capacity) : StaticArray<char>(capacity + 1)
	{
		Append(string);
	}
	StaticString(int capacity) : StaticArray<char>(capacity) {}
	~StaticString()
	{
		cout << "\nStaticString 소멸\n";
	}
	void Append(const char* string)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		for (size_t i = 0; string[i] != '\0'; i++)
		{
			this->Add(string[i]);
		}
		this->Add('\0');
	}
	void Append(const char& singleChar)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		this->Add(singleChar);
		this->Add('\0');
	}
	void AddNullChar()
	{
		this->Add('\0');
	}
	char* GetCharPointer()
	{
		return &(*this)[0];
	}
	char* GetNewCharInHeap()
	{
		int count = this->GetCount();
		char* charArray = new char[count];
		for (int i = 0; i < count; i++)
		{
			charArray[i] = (*this)[i];
		}
		return charArray;
	}
	void CopyChar(char* target, int length)
	{
		for (int i = 0; i < length; i++)
		{
			target[i] = (*this)[i];
		}
	}
	void CopyChar(char* target)
	{
		int count = this->GetCount();
		for (int i = 0; i < count; i++)
		{
			target[i] = (*this)[i];
		}
	}
};

template<typename T>
class DynamicStack : public DynamicArray<T>
{
public:
	DynamicStack(int capacity) : DynamicArray<T>(capacity) {};
	~DynamicStack()
	{
		cout << "\nStack 소멸\n";
	};
	void Push(T& element)
	{
		this->Add(element);
	}
	void PushByValue(T element)
	{
		this->Add(element);
	}
	T* TopPointer()
	{
		if (this->GetCount() > 0)
		{
			return (*this)[this->GetCount() - 1];
		}
		else
		{
			return nullptr;
		}
	}
	T TopByValue()
	{
		if (this->GetCount() > 0)
		{
			return (*this)[this->GetCount() - 1];
		}
		else
		{
			return 0;
		}
	}
};

template<typename T>
class StaticQueue
{
public:
	StaticQueue(int length)
	{
		this->length = length;
		data = (T*)malloc(sizeof(T) * length);
	}
	~StaticQueue()
	{
		delete data;
		cout << "\nStaticQueue 소멸\n";
	}
	void Enqueue(T value)
	{
		if (IsFull())
		{
			ThrowOutOfRange;
		}
		count++;
		data[enqueueIndex] = value;
		enqueueIndex = GetNextIndex(enqueueIndex);
	}
	T Dequeue()
	{
		if (IsEmpty())
		{
			ThrowOutOfRange;
		}
		count--;
		T value = data[dequeueIndex];
		dequeueIndex = GetNextIndex(dequeueIndex);
		return value;
	}
	bool IsEmpty()
	{
		if (dequeueIndex == enqueueIndex && count == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool IsFull()
	{
		if (dequeueIndex == enqueueIndex && count > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	int GetLength()
	{
		return length;
	}
	int GetCount()
	{
		return count;
	}
	int GetNextIndex(int currentIndex)
	{
		return (currentIndex + 1) % length;
	}
private:
	int length;
	int count = 0;
	int dequeueIndex = 0;
	int enqueueIndex = 0;
	T* data;
};

template<typename T>
class DynamicInfiniteQueue : public DynamicArray<T>
{
public:
	DynamicInfiniteQueue(int capacity) : DynamicArray<T>(capacity) {}
	~DynamicInfiniteQueue()
	{
		cout << "\nDynamicInfiniteQueue 소멸\n";
	}
	void Enqueue(T& value)
	{
		this->Add(value);
	}
	void EnqueueByValue(T value)
	{
		this->Add(value);
	}
	bool IsEmpty()
	{
		if (dequeueIndex == this->insertIndex)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	T DequeueByValue()
	{
		T& value = (*this)[dequeueIndex];
		dequeueIndex++;
		return value;
	}
	T& Dequeue()
	{
		T& value = (*this)[dequeueIndex];
		dequeueIndex++;
		return value;
	}
private:
	int dequeueIndex = 0;
};

class ThreadPool
{
public:
	ThreadPool(int threadsCount, int queueCapacity) : taskQueue(DynamicInfiniteQueue<void(*)()>(queueCapacity)) , workerThreads(new thread[threadsCount]) , threadsCount(threadsCount)
	{
		runningThreadCount = threadsCount;
		for (int i = 0; i < threadsCount; i++)
		{
			//workerThreads[i] = thread(&ThreadPool::Work, this);
			workerThreads[i] = thread([this, i]() { this->Work(i); });
		}
	}
	~ThreadPool()
	{
		WaitAllThread(true);
	}
	void WaitAllThread(bool andStop)
	{
		while (!taskQueue.IsEmpty())
		{
			cv.notify_all();
		}
		//큐에 Enqueue하는거는 오직 메인 쓰레드에서만 이뤄지고 현재 waitAllThread 함수 또한 메인 쓰레드에서만 호출됨.
		//따라서 Enqueue 와 현재 함수 사이의 순서 관계가 보장이 됨.
		//그래서 Empty 상태라면 큐에 있던 모든 테스크들은 이미 쓰레드들이 Dequeue해갔다는 뜻임.
		//여기서 runningThreadCount++ 명령은 Dequeue 이전에 위치함으로 결국 Empty 상태인데 runningThreadCount == 0 이 됐다면 실제로 현재 테스크를 수행중인 쓰레드는 없다는 뜻임.
		while (true)
		{
			if (runningThreadCount == 0)
			{
				if (andStop)
				{
					stopAll = true;
					cv.notify_all();
					for (int i = 0; i < threadsCount; i++)
					{
						workerThreads[i].join();
					}
				}
				return;
			}
		}
	}
	void EnqueueTask(void(*task)())
	{
		{
			unique_lock<mutex> mutexLock(lock);
			taskQueue.Enqueue(task);
		}
		cv.notify_one();
	}
private:
	DynamicInfiniteQueue<void(*)()> taskQueue;
	thread* workerThreads;
	int threadsCount;
	condition_variable cv;
	mutex lock;
	int runningThreadCount;
	bool stopAll = false;
	void Work(int index)
	{
		while (true)
		{
			void(*task)() = nullptr;
			{
				unique_lock<mutex> mutexLock(lock);
				while (taskQueue.IsEmpty())
				{
					runningThreadCount--;
					cv.wait(mutexLock);
					runningThreadCount++;
					if (stopAll)
					{
						return;
					}
				}
				task = taskQueue.Dequeue();
			}
			task();
		}
	}
};