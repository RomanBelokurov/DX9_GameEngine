
#ifndef SPEARRAY_H
#define SPEARRAY_H

////////////////////////////////////////////////////////////////////////////
//	Dynamic array class for SPE
//		Copyright(c) spehome.com, All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////


#define del_arr(p) { if(p) { delete[] (p);   (p)=0; } }

template <class T>
struct SPEArray
{
protected:
	T	*p;
public:
	int capacity;
	int size;
	
	SPEArray()
	{
		p=0;
		size=0;
		capacity=0;
	}
	SPEArray(int initsize)
	{
		p=0;
		size=0;
		capacity=0;
		if(initsize>0) Init(initsize);
	}
	SPEArray(SPEArray<T>& a)
	{
		p=0;
		size=0;
		capacity=0;
		*this=a;
	}
	~SPEArray()
	{
		del_arr(p);
		size=0;
	}
	void Init(int initsize=2)
	{
		capacity=initsize;
		size=0;
		del_arr(p);
		p=new T[capacity];
	}
	T& operator[](int i)
	{
		return p[i];
	}
	SPEArray<T>& operator=(SPEArray<T>& a)
	{
		if(&a==this)return *this;
		resize(a.capacity );
		for(int i=0;i<a.size;i++)
			p[i]=a[i];
		size=a.size;

		return *this;
	}
	void push(T& t)
	{
		if(p==0 || capacity==0)
			Init();
		else if(size==capacity)
		{
			resize(capacity*3/2);
		}
		p[size]=t;
		size++;
	}
	void push(const T& t)
	{
		if(p==0 || capacity==0)
			Init();
		else if(size==capacity)
		{
			resize(capacity*3/2);
		}
		p[size]=t;
		size++;
	}
	void push()
	{
		T t;
		push(t);
	}
	void push(SPEArray<T>& a)
	{
		int i;
		for(i=0; i<a.size; i++)
			push(a[i]);
	}
	T& pop()
	{
		return p[--size];
	}
	T& top()
	{
		return p[size-1];
	}
	int ptop()
	{
		return size-1;
	}
	void resize(int nlen)
	{
		if(nlen>capacity)
		{
			capacity=nlen;
			T *np=new T[capacity];
			int i;
			for(i=0;i<size;i++)
				np[i]=p[i];
			del_arr(p);
			p=np;
		}
	}
	void fill(int s)
	{
		int rs=s;
		if(rs<2) rs=2;
		resize(rs);
		size=s;
	}
	void clear()
	{
		size=0;
	}
	void del(int index)
	{
		int i;
		for(i=index;i<size-1;i++)
		{
			p[i]=p[i+1];
		}
		size--;
		if(size<0)size=0;
	}
	void qdel(int index) // quick delete
	{
		p[index]=p[size-1];
		size--;
		if(size<0)size=0;
	}
	bool empty()
	{
		return size==0;
	}
	void release()
	{
		del_arr(p);
		size=0;
		capacity=0;
	}

};


#endif

