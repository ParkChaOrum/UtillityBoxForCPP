#pragma once
#include <iostream>
#include <exception>
#include <list>
#include <condition_variable>
#include <mutex>
#include <thread>
#define CacheLineSize 64
#define ThrowOutOfRange throw exception("Index is out of range.");
#define byte char
using namespace std;

template <typename T>
class DynamicArrayBlock
{
public:
	DynamicArrayBlock(size_t length)
	{
		this->length = length;
		data = (T*)malloc(sizeof(T) * length);
	}
	~DynamicArrayBlock()
	{
		free(data);
		cout << "\nDynamicBlock 소멸 \n";
	}
	T& operator[](size_t index)
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
	size_t GetLength()
	{
		return length;
	}
private:
	size_t length;
	T* data;
};

template <typename T>
class DynamicArray
{
public:
	DynamicArray(size_t capacity) : capacity(capacity) {}
	~DynamicArray()
	{
		cout << "\nDynamicArray 소멸 \n";
	}
	size_t GetCount()
	{
		return insertIndex;
	}
	/// <summary>
	/// 최대한 Pop을 이용해주세요.
	/// </summary>
	/// <returns></returns>
	T PopByValue()
	{
		return Pop();
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
		T element = T(args...);
		Add(element);
	}
	/// <summary>
	/// 총 두 번의 값복사가 일어납니다.
	/// 최대한 Add 함수를 이용해주세요.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		Add(element);
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
	T& operator[](size_t index)
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
	size_t GetCapacity()
	{
		return capacity;
	}
protected:
	size_t insertIndex = 0;
	size_t capacity;
	list<DynamicArrayBlock<T>> arrayBlocks;
	T& GetRefOfElement(size_t index)
	{
		size_t localIndex;
		DynamicArrayBlock<T>& block = GetBlockByGlobalIndex(arrayBlocks, index, &localIndex);
		return block[localIndex];
	}
	size_t GetTotalLength()
	{
		return capacity * arrayBlocks.size();
	}
	size_t GetLocalIndex(size_t globalIndex, size_t blockIndex)
	{
		return globalIndex - (capacity * blockIndex);
	}
	DynamicArrayBlock<T>& GetBlockByGlobalIndex(list<DynamicArrayBlock<T>>& arrayBlocks, size_t globalIndex, size_t* localIndex)
	{
		size_t blockIndex = globalIndex / capacity;
		*localIndex = GetLocalIndex(globalIndex, blockIndex);
		if (blockIndex < (arrayBlocks.size() / 2))
		{
			typename list<DynamicArrayBlock<T>>::iterator forwardIterator = arrayBlocks.begin();
			for (size_t i = 0; i < blockIndex; i++)
			{
				forwardIterator++;
			}
			return *forwardIterator;
		}
		else
		{
			typename list<DynamicArrayBlock<T>>::reverse_iterator backwardIterator = arrayBlocks.rbegin();
			size_t loopCount = arrayBlocks.size() - blockIndex - 1;
			for (size_t i = 0; i < loopCount; i++)
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
	StaticArray(T* originalArray, size_t arrayLength) : capacity(arrayLength), data(originalArray), insertIndex(arrayLength) {}
	StaticArray(size_t capacity, T* originalArray, size_t arrayLength) : capacity(capacity), data(originalArray), insertIndex(arrayLength) {}
	StaticArray(size_t capacity) : capacity(capacity)
	{
		data = (T*)malloc(sizeof(T) * capacity);
		cout << "\nStaticArray 생성\n";
	}
	~StaticArray()
	{
		free(data);
		cout << "\nStaticArray 소멸\n";
	}
	T& operator[](size_t index)
	{
		if (index < 0 || index >= insertIndex)
		{
			ThrowOutOfRange;
		}
		else
		{
			return data[index];
		}
	}
	/// <summary>
	/// 최대한 Pop을 이용해주세요.
	/// </summary>
	/// <returns></returns>
	T PopByValue()
	{
		return Pop();
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
			return data[insertIndex];
		}
	}
	template<typename... Args>
	void AddByArgs(const Args... args)
	{
		T element = T(args...);
		Add(element);
	}
	/// <summary>
	/// 총 두 번의 값복사가 일어납니다.
	/// 최대한 Add 함수를 이용해주세요.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		Add(element);
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
		data[insertIndex] = element;
		insertIndex++;
	}
	void Clear()
	{
		insertIndex = 0;
	}
	size_t GetCount()
	{
		return insertIndex;
	}
	size_t GetCapacity()
	{
		return capacity;
	}
	StaticArray(StaticArray<T>& source)
	{
		cout << "\nStaticArray 복사 생성\n";
		insertIndex = source.insertIndex;
		capacity = source.capacity;
		data = (T*)malloc(sizeof(T) * capacity);
		for (size_t i = 0; i < source.insertIndex; i++)
		{
			(*this)[i] = source[i];
		}
	}
	void operator=(StaticArray<T>& source)
	{
		cout << "\nStaticArray 대입\n";
		insertIndex = source.insertIndex;
		capacity = source.capacity;
		if (data != nullptr)
		{
			free(data);
		}
		data = (T*)malloc(sizeof(T) * capacity);
		for (size_t i = 0; i < source.insertIndex; i++)
		{
			(*this)[i] = source[i];
		}
	}
protected:
	size_t insertIndex = 0;
	size_t capacity;
	T* data;
};

class DynamicString : public DynamicArray<char>
{
public:
	DynamicString(size_t capacity) : DynamicArray<char>(capacity) {}
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
		size_t count = this->GetCount();
		char* charArray = new char[count];
		for (size_t i = 0; i < count; i++)
		{
			charArray[i] = (*this)[i];
		}
		return *charArray;
	}
	void CopyChar(char* target, size_t length)
	{
		for (size_t i = 0; i < length; i++)
		{
			target[i] = (*this)[i];
		}
	}
};

class StaticString : public StaticArray<char>
{
public:
	//\0 때문에 혹시 실수할까봐 붙여준 상수 +1
	StaticString(const char* string, size_t capacity) : StaticArray<char>(capacity + 1)
	{
		Append(string);
	}
	StaticString(size_t capacity) : StaticArray<char>(capacity) {}
	~StaticString()
	{
		cout << "\nStaticString 소멸\n";
	}
	void Append(DynamicString& string)
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
	void Append(StaticString& string)
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
		size_t count = this->GetCount();
		char* charArray = new char[count];
		for (size_t i = 0; i < count; i++)
		{
			charArray[i] = (*this)[i];
		}
		return charArray;
	}
	void CopyChar(char* target, size_t length)
	{
		for (size_t i = 0; i < length; i++)
		{
			target[i] = (*this)[i];
		}
	}
	void CopyChar(char* target)
	{
		size_t count = this->GetCount();
		for (size_t i = 0; i < count; i++)
		{
			target[i] = (*this)[i];
		}
	}
};

template<typename T>
class DynamicStack : public DynamicArray<T>
{
public:
	DynamicStack(size_t capacity) : DynamicArray<T>(capacity) {};
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
class StaticStack : public StaticArray<T>
{
public:
	StaticStack(size_t capacity) : StaticArray<T>(capacity) {};
	~StaticStack()
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
	bool IsEmpty()
	{
		if (this->GetCount() == 0)
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
		if (this->GetCount() == this->capacity)
		{
			return true;
		}
		else
		{
			return false;
		}
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
	StaticQueue(size_t length)
	{
		this->length = length;
		data = (T*)malloc(sizeof(T) * length);
	}
	~StaticQueue()
	{
		free(data);
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
	size_t GetLength()
	{
		return length;
	}
	size_t GetCount()
	{
		return count;
	}
	size_t GetNextIndex(size_t currentIndex)
	{
		return (currentIndex + 1) % length;
	}
private:
	size_t length;
	size_t count = 0;
	size_t dequeueIndex = 0;
	size_t enqueueIndex = 0;
	T* data;
};

template<typename T>
class DynamicQueue
{
public:
	DynamicQueue(size_t capacity) : capacity(capacity)
	{
		arrayBlocks.emplace_back(capacity);
	}
	~DynamicQueue()
	{
		cout << "\nDynamicQueue 소멸\n";
	}
	void Enqueue(T& value)
	{
		if (enqueueIndexInBlock >= capacity)
		{
			arrayBlocks.emplace_back(capacity);
			enqueueIndexInBlock = 0;
		}
		arrayBlocks.back()[enqueueIndexInBlock] = value;
		enqueueIndexInBlock++;
		count++;
	}
	void EnqueueByValue(T value)
	{
		Enqueue(value);
	}
	T& Dequeue()
	{
		if (dequeueIndexInBlock >= capacity)
		{
			arrayBlocks.pop_front();
			dequeueIndexInBlock = 0;
		}
		T& value = arrayBlocks.front()[dequeueIndexInBlock];
		dequeueIndexInBlock++;
		count--;
		return value;
	}
	T DequeueByValue()
	{
		return Dequeue();
	}
	bool IsEmpty()
	{
		if (count == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	size_t GetCount()
	{
		return count;
	}
private:
	size_t enqueueIndexInBlock = 0;
	size_t dequeueIndexInBlock = 0;
	size_t count = 0;
	size_t capacity;
	list<DynamicArrayBlock<T>> arrayBlocks;
};

template<typename T>
class InfiniteChainQueue : public DynamicArray<T>
{
public:
	InfiniteChainQueue(size_t capacity) : DynamicArray<T>(capacity) {}
	~InfiniteChainQueue()
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
private:
	size_t dequeueIndex = 0;
};

class alignas(CacheLineSize) StaticHeapMemoryPool
{
public:
	//Alloc은 최대한 64의 배수로
	StaticHeapMemoryPool(size_t interval, size_t count) :
		interval(interval)
		, basePTR((byte*)malloc(interval* count))
		, indexStack(count)
	{
		ResetStack();
		cout << "\nMemoryPool 생성\n";
	}
	~StaticHeapMemoryPool()
	{
		cout << "\nMemoryPool 소멸\n";
		free(basePTR);
	}
	void* Malloc()
	{
		return (basePTR + (indexStack.Pop() * interval));
	}
	byte* MallocByByte()
	{
		return (basePTR + (indexStack.Pop() * interval));
	}
	void Free(void* ptr)
	{
		indexStack.PushByValue(GetIndex((byte*)ptr));
	}
	size_t GetIndex(byte* ptr)
	{
		return ((size_t)ptr - (size_t)basePTR) / interval;
	}
	void Clear()
	{
		ResetStack();
	}
	bool IndexStackIsEmpty()
	{
		return indexStack.IsEmpty();
	}
	byte* GetBasePTR()
	{
		return basePTR;
	}
private:
	mutex lock;
	size_t interval;
	StaticStack<size_t> indexStack;
	byte* basePTR = nullptr;
	void ResetStack()
	{
		size_t count = indexStack.GetCapacity();
		for (size_t i = 0; i < count; i++)
		{
			indexStack.PushByValue(count - i - 1);
		}
	}
};

#define HeaderSize 8
class alignas(CacheLineSize) DynamicHeapMemoryPool
{
public:
	//Without header
	DynamicHeapMemoryPool(size_t interval, size_t capacity) : interval(interval + HeaderSize), capacity(capacity)
	{
		EmplaceNewMemBlock();
	}
	~DynamicHeapMemoryPool()
	{
		cout << "\nDynamicHeapMemoryPool 소멸" << endl;
	}
	void* Malloc()
	{
		list<StaticHeapMemoryPool>::iterator blocksIterator = memoryBlocks.begin();
		while ((*blocksIterator).IndexStackIsEmpty())
		{
			blocksIterator++;
			if (blocksIterator == memoryBlocks.end())
			{
				EmplaceNewMemBlock();
				return (memoryBlocks.back().MallocByByte() + HeaderSize);
			}
		}
		return ((*blocksIterator).MallocByByte() + HeaderSize);
	}
	void* MallocWithLock()
	{
		unique_lock<mutex> mutexLock(lock);
		return Malloc();
	}
	void Free(void* ptr)
	{
		byte* startPTR = (byte*)ptr - HeaderSize;
		size_t blockIndex = *(size_t*)startPTR;
		GetMemoryBlock(blockIndex).Free(startPTR);
	}
	void FreeWithLock(void* ptr)
	{
		unique_lock<mutex> mutexLock(lock);
		Free(ptr);
	}
private:
	mutex lock;
	size_t interval;
	size_t capacity;
	list<StaticHeapMemoryPool> memoryBlocks;
	void EmplaceNewMemBlock()
	{
		memoryBlocks.emplace_back(interval, capacity);
		ResetHeader(memoryBlocks.size() - 1, memoryBlocks.back().GetBasePTR());
	}
	void ResetHeader(size_t headerNumber, byte* basePTR)
	{
		for (size_t i = 0; i < capacity; i++)
		{
			*(size_t*)(basePTR + (interval * i)) = headerNumber;
		}
	}
	StaticHeapMemoryPool& GetMemoryBlock(size_t blockIndex)
	{
		if (blockIndex < (memoryBlocks.size() / 2))
		{
			typename list<StaticHeapMemoryPool>::iterator forwardIterator = memoryBlocks.begin();
			for (size_t i = 0; i < blockIndex; i++)
			{
				forwardIterator++;
			}
			return *forwardIterator;
		}
		else
		{
			typename list<StaticHeapMemoryPool>::reverse_iterator backwardIterator = memoryBlocks.rbegin();
			size_t loopCount = memoryBlocks.size() - blockIndex - 1;
			for (size_t i = 0; i < loopCount; i++)
			{
				backwardIterator++;
			}
			return *backwardIterator;
		}
	}
};

class alignas(CacheLineSize) StackMemoryPool
{
public:
	//Alloc은 최대한 64의 배수로
	StackMemoryPool(size_t size) :
		size(size)
		, basePTR((byte*)malloc(size))
	{
		cout << "\nStackMemoryPool 생성\n";
	}
	~StackMemoryPool()
	{
		cout << "\nStackMemoryPool 소멸\n";
		free(basePTR);
	}
	template<typename T>
	T* Malloc(size_t count)
	{
		size_t initAvailableSize = size - reservedSize;
		size_t postAvailableSize = initAvailableSize;
		size_t mallocSize = sizeof(T) * count;
		void* ptr = (void*)(basePTR + reservedSize);
		if (align(alignof(T), mallocSize, ptr, postAvailableSize))
		{
			size_t padding = initAvailableSize - postAvailableSize;
			reservedSize += mallocSize + padding;
			return (T*)ptr;
		}
		else
		{
			ThrowOutOfRange;
		}
	}
	template<typename T>
	T* MallocWithLock(size_t count)
	{
		unique_lock<mutex> mutexLock(lock);
		Malloc(count);
	}
	template<typename T>
	void SetValue(T* ptr, T value)
	{
		*ptr = value;
	}
	template<typename T>
	void SetValueWithLock(T* ptr, T value)
	{
		unique_lock<mutex> mutexLock(lock);
		SetValue(ptr, value);
	}
	template<typename T>
	T GetValue(T* ptr)
	{
		return *ptr;
	}
	template<typename T>
	T GetValueWithLock(T* ptr)
	{
		unique_lock<mutex> mutexLock(lock);
		return GetValue(ptr);
	}
	void Clear()
	{
		reservedSize = 0;
	}
	void operator=(const StackMemoryPool& source)
	{
		basePTR = source.basePTR;
		size = source.size;
		reservedSize = source.reservedSize;
	}
private:
	mutex lock;
	size_t reservedSize = 0;
	size_t size = 0;
	byte* basePTR = nullptr;
};

class ThreadPool
{
public:
	ThreadPool(size_t threadsCount, size_t queueCapacity, size_t sharedMemorySize, size_t threadPrivateMemorySize, size_t captureMemoryInterval, size_t captureMemoryCapacity) :
		threadsCount(threadsCount)
		, runningThreadCount(threadsCount)
		, taskQueue(queueCapacity)
		, sharedMemory(sharedMemorySize)
		, workerThreads((thread*)malloc(sizeof(thread)* threadsCount))
		, threadPrivateMemory((StackMemoryPool*)malloc(sizeof(StackMemoryPool)* threadsCount))
		, captureMemory(captureMemoryInterval, captureMemoryCapacity)
		, captureQueue(queueCapacity)
	{
		for (size_t i = 0; i < threadsCount; i++)
		{
			new (workerThreads + i) thread([i, this]() { this->Work(i); });
			new (threadPrivateMemory + i) StackMemoryPool(threadPrivateMemorySize);
		}
	}
	~ThreadPool()
	{
		if (!stopAll)
		{
			WaitAllThread(true);
		}
		for (size_t i = 0; i < threadsCount; i++)
		{
			workerThreads[i].~thread();
		}
		free(workerThreads);
		for (size_t i = 0; i < threadsCount; i++)
		{
			threadPrivateMemory[i].~StackMemoryPool();
		}
		free(threadPrivateMemory);
		cout << "\nThreadPool 소멸\n";
	}
	void WaitAllThread(bool andStop)
	{
		while (!taskQueue.IsEmpty()) {}
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
					for (size_t i = 0; i < threadsCount; i++)
					{
						workerThreads[i].join();
					}
				}
				return;
			}
		}
	}
	void EnqueueTask(void(*task)(size_t, ThreadPool*, void*), void* capturePTR)
	{
		{
			unique_lock<mutex> mutexLock(lock);
			taskQueue.Enqueue(task);
			captureQueue.Enqueue(capturePTR);
		}
		cv.notify_one();
	}
	StackMemoryPool sharedMemory;
	StackMemoryPool* threadPrivateMemory;
	DynamicHeapMemoryPool captureMemory;
private:
	DynamicQueue<void(*)(size_t, ThreadPool*, void*)> taskQueue;
	DynamicQueue<void*> captureQueue;
	thread* workerThreads;
	size_t threadsCount;
	condition_variable cv;
	mutex lock;
	atomic_size_t runningThreadCount;
	bool stopAll = false;
	void Work(size_t threadIndex)
	{
		while (true)
		{
			void(*task)(size_t, ThreadPool*, void*) = nullptr;
			void* capturePTR = nullptr;
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
				capturePTR = captureQueue.Dequeue();
			}
			task(threadIndex, this, capturePTR);
			if (capturePTR != nullptr)
			{
				captureMemory.FreeWithLock(capturePTR);
			}
		}
	}
};

struct alignas(CacheLineSize) Flag
{
public:
	bool value;
	void operator=(const Flag& sourceFlag)
	{
		this->value = sourceFlag.value;
	}
	void operator=(bool value)
	{
		this->value = value;
	}
	operator bool() const
	{
		return value;
	}
	Flag()
	{
		value = false;
	}
	Flag(bool value) : value(value) {}
	Flag(const Flag& sourceFlag) : value(sourceFlag.value) {}
};
