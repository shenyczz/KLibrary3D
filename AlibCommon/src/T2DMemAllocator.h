/******************************************************************************
 ** T2DMemAllocator.h

 ** Announce: if it works, I know it was written by
                ShenYongchen(shenyczz@163.com),lives in ZhengZhou,HeNan.
              if not, then I don't know nothing.

 **     Name: T2DMemAllocator - 二维内存分配器类模板
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
		typedef T2DMemAllocator <float>	CFloat2DmemAllocator;
	//2. Allocate Memory
		CFloat2DmemAllocator::AllocMemory2D(_Ty ** &p,int rows,int cols);
	//3. Free Memory
		CFloat2DmemAllocator::FreeMemory2D(_Ty **&pp);

******************************************************************************/

#ifndef __INCLUDED_TEMPLATE_2DMEMALLOCATOR_H__
#define __INCLUDED_TEMPLATE_2DMEMALLOCATOR_H__

template <typename _Ty>
class T2DMemAllocator 
{
public:
	T2DMemAllocator() {}
	virtual ~T2DMemAllocator() {}

public:
	bool AllocMemory(_Ty ** &pp, int rows, int cols)
	{
		return _AllocMemory2D(pp,rows,cols);
	}
	void FreeMemory(_Ty ** &pp)
	{
		_FreeMemory2D(pp);
	}

public:
	static bool AllocMemory2D(_Ty ** &pp, int rows, int cols)
	{
		return _AllocMemory2D(pp,rows,cols);
	}
	static void FreeMemory2D(_Ty **&pp)
	{
		_FreeMemory2D(pp);
	}

private:
	static bool _AllocMemory2D(_Ty ** &pp, int rows, int cols);
	static void _FreeMemory2D(_Ty **&pp);
};

//分配2D内存
template <typename _Ty>
bool T2DMemAllocator<_Ty>::_AllocMemory2D(_Ty ** &pp, int rows, int cols)
{
	_Ty **p2 = NULL;

	p2=new _Ty*[rows];
	if (!p2)
	{
		//TRACE("Alloc memory Open error!!\n");
		return false;
	}
	
	//allocate a big enough block
	p2[0]=new _Ty [rows*cols];
	if (!p2[0])
	{
		//TRACE("memeroy Open error!!\n");
		return false;
	}

	memset(p2[0],0,sizeof(_Ty)*rows*cols);

	for(int i=1;i<rows;i++)
	{
		p2[i]=p2[0]+(long)cols*i;
	}

	pp = p2;
	
	return true;
}

//释放2D内存
template <typename _Ty>
void T2DMemAllocator<_Ty>::_FreeMemory2D(_Ty **&pp)
{
	//TRACE("in FreeMemory2D\n");
	if(pp)
	{//释放顺序::一维指针-->二维指针
		
		delete []pp[0];
		pp[0] = 0;
		
		delete []pp;
		pp = 0;
	}
}

typedef T2DMemAllocator<BYTE>		KByte2DMemAllocator;
typedef T2DMemAllocator<WORD>		KWord2DMemAllocator;
typedef T2DMemAllocator<SHORT>		KShort2DMemAllocator;
typedef T2DMemAllocator<LONG>		KLong2DMemAllocator;
typedef T2DMemAllocator<FLOAT>		KFloat2DMemAllocator;
typedef T2DMemAllocator<DOUBLE>		KDouble2DMemAllocator;

#endif // #ifndef __INCLUDED_TEMPLATE_2DMEMALLOCATOR_H__
