#pragma once
#include <iostream>
#include <exception>
#include <list>
#include <condition_variable>
#include <mutex>
#include <functional>
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
		cout << "\nDynamicBlock �Ҹ� \n";
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
		cout << "\nDynamicArray �Ҹ� \n";
	}
	size_t GetCount()
	{
		return insertIndex;
	}
	/// <summary>
	/// �ִ��� Pop�� �̿����ּ���.
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
	/// �� �� ���� �����簡 �Ͼ�ϴ�.
	/// �ִ��� Add �Լ��� �̿����ּ���.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		Add(element);
	}
	/// <summary>
	/// ���� ������ ���ǿ� ���� ������ �Ͼ.
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
		cout << "\nStaticArray ����\n";
	}
	~StaticArray()
	{
		free(data);
		cout << "\nStaticArray �Ҹ�\n";
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
	/// �ִ��� Pop�� �̿����ּ���.
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
	/// �� �� ���� �����簡 �Ͼ�ϴ�.
	/// �ִ��� Add �Լ��� �̿����ּ���.
	/// </summary>
	/// <param name="element"></param>
	void AddByValue(const T element)
	{
		Add(element);
	}
	/// <summary>
	/// ���� ������ ���ǿ� ���� ������ �Ͼ.
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
		cout << "\nStaticArray ���� ����\n";
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
		cout << "\nStaticArray ����\n";
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

class StaticString : public StaticArray<char>
{
public:
	//\0 ������ Ȥ�� �Ǽ��ұ�� �ٿ��� ��� +1
	StaticString(const char* string, size_t capacity) : StaticArray<char>(capacity + 1)
	{
		Append(string);
	}
	StaticString(size_t capacity) : StaticArray<char>(capacity + 1) {}
	~StaticString()
	{
		cout << "\nStaticString �Ҹ�\n";
	}
	//void Append(DynamicString& string)
	//{
	//	if (this->GetCount() > 0)
	//	{
	//		this->Pop();
	//	}
	//	if (string.GetCount() > 0)
	//	{
	//		for (size_t i = 0; string[i] != '\0'; i++)
	//		{
	//			this->Add(string[i]);
	//		}
	//	}
	//	this->Add('\0');
	//}
	void Append(StaticString& string)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		if (string.GetCount() > 0)
		{
			for (size_t i = 0; string[i] != '\0'; i++)
			{
				this->Add(string[i]);
			}
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

class DynamicString : public DynamicArray<char>
{
public:
	DynamicString(size_t capacity) : DynamicArray<char>(capacity) {}
	~DynamicString()
	{
		cout << "\nDynamicString �Ҹ�\n";
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
	void Append(StaticString& string)
	{
		if (this->GetCount() > 0)
		{
			this->Pop();
		}
		size_t count = string.GetCount();
		for (size_t i = 0; (i < count && string[i] != '\0'); i++)
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

template<typename T>
class DynamicStack : public DynamicArray<T>
{
public:
	DynamicStack(size_t capacity) : DynamicArray<T>(capacity) {};
	~DynamicStack()
	{
		cout << "\nStack �Ҹ�\n";
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
		cout << "\nStack �Ҹ�\n";
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
		cout << "\nStaticQueue �Ҹ�\n";
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
		cout << "\nDynamicQueue �Ҹ�\n";
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
		cout << "\nDynamicInfiniteQueue �Ҹ�\n";
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
	//Alloc�� �ִ��� 64�� �����
	StaticHeapMemoryPool(size_t interval, size_t count) :
		interval(interval)
		, basePTR((byte*)malloc(interval* count))
		, indexStack(count)
	{
		ResetStack();
		cout << "\nMemoryPool ����\n";
	}
	~StaticHeapMemoryPool()
	{
		cout << "\nMemoryPool �Ҹ�\n";
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
		cout << "\nDynamicHeapMemoryPool �Ҹ�" << endl;
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
	//Alloc�� �ִ��� 64�� �����
	StackMemoryPool(size_t size) :
		size(size)
		, basePTR((byte*)malloc(size))
	{
		cout << "\nStackMemoryPool ����\n";
	}
	~StackMemoryPool()
	{
		cout << "\nStackMemoryPool �Ҹ�\n";
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
		cout << "\nThreadPool �Ҹ�\n";
	}
	void WaitAllThread(bool andStop)
	{
		while (!taskQueue.IsEmpty()) {}
		//ť�� Enqueue�ϴ°Ŵ� ���� ���� �����忡���� �̷����� ���� waitAllThread �Լ� ���� ���� �����忡���� ȣ���.
		//���� Enqueue �� ���� �Լ� ������ ���� ���谡 ������ ��.
		//�׷��� Empty ���¶�� ť�� �ִ� ��� �׽�ũ���� �̹� ��������� Dequeue�ذ��ٴ� ����.
		//���⼭ runningThreadCount++ ����� Dequeue ������ ��ġ������ �ᱹ Empty �����ε� runningThreadCount == 0 �� �ƴٸ� ������ ���� �׽�ũ�� �������� ������� ���ٴ� ����.
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