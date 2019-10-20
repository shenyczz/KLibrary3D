/******************************************************************************
 ** T2DMemory.h

 ** Announce: if it works, I know it was written by
                ShenYongchen(shenyczz@163.com),lives in ZhengZhou,HeNan.
              if not, then I don't know nothing.

 **     Name: T2DMemory - 二维内存类模板
 **  Version: 
 ** Function: 
 **  Explain: 

 **   Author: 申永辰.郑州 (shenyczz@163.com)
 ** DateTime: 2000 -
 ** Web Site: 

 ** Modifier: 
 ** DateTime: 
 **  Explain: 

 **    Usage: 

	//1. Declare Template Class
		T2DMemory<float>	f2dMemory;
	//2. Allocate Memory
		f2dMemory.AllocMemory(l,r,c);
	//3. Free Memory
		f2dMemory.FreeMemory();

	//4. Use it
		float fv = f2dMemory(0,0);
		f2dMemory(1,1) = fv;
		f2dMemory(1,1) += 0;

******************************************************************************/

#ifndef __INCLUDED_TEMPLATE_T2DMEMORY_H__
#define __INCLUDED_TEMPLATE_T2DMEMORY_H__

template <typename _Ty>
class T2DMemory 
{
public:
	T2DMemory()
		:m_pv(0)
		,m_rows(0)
		,m_cols(0) 
	{
	}

	virtual ~T2DMemory()
	{
		//_deletea(m_pv);
	}

public:
	bool AllocMemory(int rows,int cols)
	{
		if(rows<=0 || cols<=0)
			return false;

		m_pv = new _Ty [rows*cols];
		if(!m_pv)
			return false;

		memset(m_pv,0,sizeof(_Ty)*rows*cols);

		m_rows = rows;
		m_cols = cols;

		return true;
	}

	void FreeMemory()
	{
		_deletea(m_pv);
	}

	void GetSize(int& row,int& col)
	{
		row = m_rows;
		col = m_cols;
	}

public:
	//[操作符重载] -> 获取元素(i,j)的值,左值函数(可写入)
	_Ty& operator()(int row, int col)
	{
		if(row<0 || row>=m_rows)
			return *m_pv;

		if(col<0 || col>=m_cols)
			return *m_pv;

		return *( m_pv + row*m_cols+col );
	}

	//[操作符重载] -> 获取元素(i,j)的值,右值函数(只读)
	const _Ty& operator()(int row, int col) const
	{
		if(row<0 || row>=m_rows)
			return *m_pv;

		if(col<0 || col>=m_cols)
			return *m_pv;

		return *( m_pv + row*m_cols+col );
	}

private:
	_Ty * m_pv;
	int m_rows;
	int m_cols;
};

typedef T2DMemory<BYTE>		KByte2DMemory;
typedef T2DMemory<WORD>		KWord2DMemory;
typedef T2DMemory<SHORT>	KShort2DMemory;
typedef T2DMemory<LONG>		KLong2DMemory;
typedef T2DMemory<FLOAT>	KFloat2DMemory;
typedef T2DMemory<DOUBLE>	KDouble2DMemory;

#endif // #ifndef __INCLUDED_TEMPLATE_T2DMEMORY_H__
