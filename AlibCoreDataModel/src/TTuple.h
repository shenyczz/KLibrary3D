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


template<typename _Ty, int size>
class TTuple
{
public:
	TTuple()
	{
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
		for (int i = 0; i < size; ++i)
		{
			this->Data[i] = init[i];
		}
	}

	int GetSize() const { return size; }

	_Ty* GetData() { return this->Data; }
	const _Ty* GetData() const { return this->Data; }


	_Ty& operator[](int i) { return this->Data[i]; }
	const _Ty& operator[](int i) const { return this->Data[i]; }

	_Ty operator()(int i) const
	{
		assert("pre: index_in_bounds" && i >= 0 && i < size);
		return this->Data[i];
	}

	bool Compare(const TTuple<_Ty, size>& other, const _Ty& tol) const
	{
		if (size != other.GetSize())
		{
			return false;
		}
		for (int i = 0; i < size; ++i)
		{
			if (std::abs(this->Data[i] - other.Data[i]) >= tol)
			{
				return false;
			}
		}
		return true;
	}

	template<typename _xty>
	TTuple<_xty, size> Cast() const
	{
		TTuple<_xty, size> result;
		for (int i = 0; i < size; ++i)
		{
			result[i] = static_cast<_xty>(this->Data[i]);
		}
		return result;
	}

protected:
	_Ty Data[size];

};


ALIBAPI(void) TTuple_test();


