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
		cout << "\n���̳��� ��� �Ҹ� \n";
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
		cout << "\n���̳��� ��� �Ҹ� \n";
	}
	int GetCount()
	{
		return insertIndex;
	}
	/// <summary>
	/// �ִ��� Pop�� �̿����ּ���.
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
	/// �� �� ���� �����簡 �Ͼ�ϴ�.
	/// �ִ��� Add �Լ��� �̿����ּ���.
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
private:
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
		if (blockIndex < (arrayBlocks.size() / 2))
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

class DynamicString
{
public:
	DynamicString(int capacity)
	{
		totalString = new DynamicArray<char>(capacity);
	}
	~DynamicString()
	{
		delete totalString;
		cout << "\nStringBuilder �Ҹ�\n";
	}
	void Append(const char* string)
	{
		if (totalString->GetCount() > 0)
		{
			totalString->Pop();
		}
		for (size_t i = 0; string[i] != '\0'; i++)
		{
			totalString->Add(string[i]);
		}
		totalString->Add('\0');
	}
	void Append(const char& singleChar)
	{
		if (totalString->GetCount() > 0)
		{
			totalString->Pop();
		}
		totalString->Add(singleChar);
		totalString->Add('\0');
	}
	char* GetCharArray()
	{
		int count = totalString->GetCount();
		char* charArray = new char[count];
		for (size_t i = 0; i < count; i++)
		{
			charArray[i] = (*totalString)[i];
		}
		return charArray;
	}
	int GetCount()
	{
		return totalString->GetCount();
	}
	void Clear()
	{
		totalString->Clear();
	}
private:
	DynamicArray<char>* totalString;
};

template<typename T>
class DyanamicStack
{
public:
	DyanamicStack(int capacity)
	{
		container = new DynamicArray<T>(capacity);
	};
	~DyanamicStack()
	{
		delete container;
		cout << "\nStack �Ҹ�\n";
	};
	void Push(T& element)
	{
		container->Add(element);
	}
	void Push(T element)
	{
		container->Add(element);
	}
	T Pop()
	{
		return container->Pop();
	}
	void Clear()
	{
		container->Clear();
	}
private:
	DynamicArray<T>* container;
};