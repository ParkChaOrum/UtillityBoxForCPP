#pragma once
#include <iostream>
#include <exception>
#include <list>
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
		cout << "\nStaticArray 소멸 \n";
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