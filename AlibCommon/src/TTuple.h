/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
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


template<typename _Ty, int size>
class TTuple
{
public:
	TTuple()
	{
		if (size > 0)
		{
			for (int i = 0; i < size; ++i)
			{
				this->Data[i] = static_cast<_Ty>(0);
			}
		}
	}

	explicit TTuple(const _Ty& scalar)
	{
		for (int i = 0; i < size; ++i)
		{
			this->Data[i] = scalar;
		}
	}

	explicit TTuple(const _Ty* init)
	{
		try
		{
			for (int i = 0; i < size; ++i)
			{
				this->Data[i] = init[i];
			}
		}
		catch (const std::exception&)
		{

		}
	}


	_Ty& operator [](int i)
	{
		assert("pre: index_in_bounds" && i >= 0 && i < size);
		return this->Data[i];
	}

	const _Ty& operator [](int i) const
	{
		assert("pre: index_in_bounds" && i >= 0 && i < size);
		return this->Data[i];
	}

	_Ty operator ()(int i) const
	{
		assert("pre: index_in_bounds" && i >= 0 && i < size);
		return this->Data[i];
	}

	int GetSize() const
	{
		return size;
	}

	_Ty* GetData()
	{
		return this->Data;
	}

	const _Ty* GetData() const
	{
		return this->Data;
	}

	// 是否相等
	bool Compare(const TTuple<_Ty, size>& other, const _Ty& tol) const
	{
		bool result = true;

		if (size == other.GetSize())
		{
			for (int i = 0; i < size; ++i)
			{
				if (std::abs(this->Data[i] - other.Data[i]) >= tol)
				{
					result = false;
					break;
				}
			}
		}

		return result;
	}

	template<typename _tu>
	TTuple<_tu, size> Cast() const
	{
		TTuple<_tu, size> result;

		for (int i = 0; i < size; ++i)
		{
			result[i] = static_cast<_tu>(this->Data[i]);
		}

		return result;
	}

protected:
	_Ty Data[size];


	//@EndOf(TTuple)
};


#ifdef _DEBUG

ALIBAPI(void) alib_test__TTuple();

#endif // _DEBUG




