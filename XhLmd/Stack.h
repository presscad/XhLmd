#pragma once

//∆’Õ®∂—’ª¿‡
template <class TYPE> class CStack
{
	char *top;
	char *bottom;
	char *cursor;
	char *work;
	size_t bytes;
	size_t blockSize;
public:
	CStack(size_t nCount=2)
	{
		if(nCount<2)
			nCount=2;
		blockSize=2;
		bytes = sizeof(TYPE);
		top = new char[nCount*bytes];
		bottom = cursor = top + nCount*bytes;
		work = NULL;
	}
	~CStack(){ delete []top; }
	BOOL IsEmpty()
	{
		if(bottom<=cursor)
			return TRUE;
		else
			return FALSE;
	}
	void Empty()
	{
		cursor = bottom;
	}
	void SetBlockSize(size_t nCount)
	{
		blockSize=nCount;
	}
	int GetRemnantSize()
	{
		return (int)(cursor-top)/(int)bytes;
	}
	int GetPushSize()
	{
		return (int)(bottom-cursor)/(int)bytes;
	}
	TYPE* TopAtom()		//∑√Œ ’ª∂•‘™Àÿ
	{
		return (TYPE*)cursor;
	}
	int push(const TYPE& val)
	{
		if(top > cursor - bytes)
		{
			size_t d=2;
			int nPos=(int)(bottom-cursor);
			int nLen=(int)(bottom-top);
			char *tm=new char[nLen+blockSize*bytes];
			memcpy(tm+blockSize*bytes,top,nLen);
			delete []top;
			top=tm;
			bottom=top+nLen+blockSize*bytes;
			cursor=bottom-nPos-bytes;
			memcpy(cursor,&val, bytes);
		}
		else
		{
			cursor -= bytes;
			memcpy(cursor, &val, bytes);
		}
		return (int)(bottom-cursor)/(int)bytes;
	}
	bool pop(TYPE& res)
	{
		if(bottom < (cursor + bytes))
			return false;
		else
		{
			memcpy(&res, cursor, bytes);
			cursor += bytes;
			return true;
		}
	}
};
