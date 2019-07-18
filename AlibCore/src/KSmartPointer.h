/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: …Í”¿≥Ω.÷£÷› (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once


template<typename T>
class KSmartPointer
{
public:
	KSmartPointer(T* p = nullptr) : _ptr(p)
	{
		if (_ptr)
		{
			_count = new size_t(1);
		}
		else
		{
			_count = new size_t(0);
		}
	}

	KSmartPointer(const KSmartPointer& other)
	{
		if (this != &other)
		{
			_ptr = other._ptr;
			_count = other._count;
			(*_count)++;
		}
	}

	~KSmartPointer()
	{
		(*_count)--;
		if (*_count == 0)
		{
			_delete(_ptr);
			_delete(_count);
		}
	}


	KSmartPointer& operator=(const KSmartPointer& other)
	{
		if (_ptr == other._ptr)
		{
			return *this;
		}

		if (_ptr)
		{
			(*_count)--;
			if (_count == 0)
			{
				_delete(_ptr);
				_delete(_count);
			}
		}

		_ptr = other._ptr;
		_count = other._count;
		(*_count)++;
		return *this;
	}

	T& operator*()
	{
		assert(_ptr == nullptr);
		return *(_ptr);

	}

	T* operator->()
	{
		assert(_ptr == nullptr);
		return this->_ptr;
	}

	T* Get()
	{
		return this->_ptr;
	}


	size_t use_count()
	{
		return *_count;
	}

public:
	//static T* New()
	//{
	//	return new T();
	//}


private:
	T* _ptr;
	size_t* _count;
};
