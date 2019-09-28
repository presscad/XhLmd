#ifndef __F_ENTTITY_LIST_H_
#ifndef __ATOM_LIST_H_
#define __ATOM_LIST_H_
//��ͨ��ջ��
//��ͨ��ջ��
template <class TYPE> class CStack
{
	char *top;
	char *bottom;
	char *cursor;
	char *work;
	size_t bytes;
	size_t blockSize;
public:
	CStack();
	CStack(size_t nCount);
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
		return (cursor-top)/bytes;
	}
	int GetPushSize()
	{
		return (bottom-cursor)/bytes;
	}
	TYPE* TopAtom()		//����ջ��Ԫ��
	{
		return (TYPE*)cursor;
	}
	int push(const TYPE& val)
	{
		if(top > cursor - bytes)
		{
			int nPos=bottom-cursor;
			int nLen=bottom-top;
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
	BOOL pop(TYPE& res)
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

template <class TYPE> 
CStack<TYPE>::CStack()
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[bytes*2];
	bottom = cursor = top + bytes*2;
	work = NULL;
}

template <class TYPE> 
CStack<TYPE>::CStack(size_t nCount)
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[nCount*bytes];
	bottom = cursor = top + nCount*bytes;
	work = NULL;
}
//����ģ����
template <class TYPE> class ATOM_LIST
{
public:
    ATOM_LIST()
	{// ���캯��
		NodeNum=0;
		index = 0;
		cursor = head = tail = NULL;
	}
    ~ATOM_LIST(){Empty();}

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		BOOL bDeleted;
		tagDATA_TYPE(){bDeleted=FALSE;}
	}DATA_TYPE;
	CStack<DATA_TYPE*> POS_STACK;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
	bool push_stack()
	{
		if(cursor)
		{
			POS_STACK.push(cursor);
			return true;
		}
		else
			return false;
	}
	bool pop_stack()
	{
		if(POS_STACK.pop(cursor))
			return true;
		else
		{
			cursor = NULL;
			return false;
		}
	}
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
    TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
	{
		if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->bDeleted = FALSE;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			index = NodeNum-1;
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		//GetTail();//cursor is equal to tail now.   --00.05.25
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		index = NodeNum-1;
		
		return &ptr->atom;
	}
    TYPE* append(const TYPE &atom)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    TYPE* insert()
	{
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;

		if(NodeNum<=0)//������
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum++;
			index = NodeNum-1;//������ڵ���Ϊ��ǰ�ڵ�
			return &ptr->atom;
		}

		//index = GetCurrentIndex();
		//cursor = &GetByIndex( index );
        	//��������ڵ���������ԭ�������ӹ�ϵ
		ptr->next = cursor;
  		ptr->prev = cursor->prev;
			//ԭ��ǰ�ڵ㴦��
		if(cursor->prev!=NULL)
    		cursor->prev->next = ptr;
		else	//cursorԭΪ�׽ڵ�
			head = ptr;
		cursor->prev = ptr;

		NodeNum++;
		return &ptr->atom;
	}
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
	{
		return index;
	}

    TYPE* GetByIndex(long ii)
	{
		if(ii<0)
    		return NULL;
		long i=0;
		TYPE *pItem;
		for(pItem=GetFirst();pItem;pItem=GetNext(),i++)
		{
			if(i==ii)
				return pItem;
		}
		return NULL;
	}
    TYPE& operator[](long ii)//���������Ż�ȡ�߶�ʵ��
	{
		return *GetByIndex(ii);
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
		{
			index++;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
	    		cursor = cursor->prev;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
		{
    		index--;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			index = 0;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			index = NodeNum-1;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}

	long GetNodeNum()const{return NodeNum;}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteCursor(BOOL bClean=FALSE)
	{
		if(cursor==NULL||cursor->bDeleted)
			return FALSE;
		else
		{
			if(bClean)
			{
				DATA_TYPE *cursor2=NULL;
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
					cursor2=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
						cursor2=head;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
						cursor2=tail;
					}
				}
				delete cursor;
				cursor=cursor2;
			}
			else
				cursor->bDeleted=TRUE;
			NodeNum--;
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
	void Clean()
	{
		for(cursor=head;cursor;)
		{
			DATA_TYPE *cursor2=cursor->next;
			if(cursor->bDeleted)
			{	//�ѱ�ɾ��Ԫ��,��λ�����
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
					}
				}
				delete cursor;
			}
			cursor=cursor2;
		}
	}
    BOOL DeleteAt(long ii)	// ɾ��һ���ڵ�
	{
		int i=0;
		for(TYPE* pType=GetFirst();pType;pType=GetNext(),i++)
		{
			if(i==ii)
				return DeleteCursor();
		}
		return FALSE;
	}
    void Destroy()		// ����������ǰ��ʵ��
	{
		~ATOM_LIST();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			delete tail;
			tail = cursor;
		}
		NodeNum=0;
		index = 0;
		head = tail = NULL;
		cursor = NULL;
	}
	void EmptyStack(){POS_STACK.Empty();}
};
#endif
#endif