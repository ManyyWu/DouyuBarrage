#pragma once
/*
* Copyright (c) 2017, K
* All rights reserved.
*
* Filename:            kqueue.h
* File identification: no
* Summary:             queue class definition(single non circulate queue)
*                      template class parameters cannot be basic data types
*
* Current version:     2.0
* Author:              K
* Finish data:         2017/4/8
*
* Replace version:     1.0
* Author:              k
* Finish data:         2016/-/-
*/

#ifndef DY_BARRAGE_KQUEUE_H
#define DY_BARRAGE_KQUEUE_H

extern "C" {
#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#endif
};

#include <cassert>
#include <new>

#ifndef _MSC_VER
#define _In_
#define _Out_
#endif

// namespace k
namespace k
{

	// template class queue
	template <class T>
	class queue
	{	public:
		// class queue_node
		class queue_node
		{

		public:
			friend class queue;

		public:
			/* Default constructure function */
			queue_node ();

			/* Distructure functions */
			~queue_node ();

		private:
			queue_node *m_p_next;

		private:
			T m_data;
	};// end of class queue_node

	public:
		/* Default constructure functions */
		queue<T>();

		/* Destructure function */
		~queue<T>();

	public:
		/* Judge whether queue is empty */
		bool
		is_empty();

		/* Get length of queue */
		int
		get_length();

	public:
		/* Pop queue head */
		T
		pop();

		/* Push node to queue */
		void
		push(_In_ T des);

		/* Clear queue */
		void
		clear_queue();

	private:
		// head pointer of queue
		typename queue<T>::queue_node *m_p_head;

		// real pointer of queue
		typename queue<T>::queue_node *m_p_tail;

		// queue length;
		int m_length;

        // mutex
#ifdef _MSC_VER
        HANDLE m_mutex;
#else
        pthread_mutex_t m_mutex;
#endif

	};// end of class queue

	/* Construct with init data */
	template <class T>
	inline
	queue<T>::queue_node::queue_node ()
	{
		this->m_p_next = nullptr;
	}// end of queue_node (T)

	/* Distructure fucntion */
	template <class T>
	inline
	queue<T>::queue_node::~queue_node ()
	{
	}// end of ~queue_node

	/* the default constructure function of class queue */
	template <class T>
	inline
	queue<T>::queue()
	{
		this->m_p_head = nullptr;
		this->m_p_tail = nullptr;
		this->m_length = 0;

        // create mutex
#ifdef _MSC_VER
        this->m_mutex = CreateMutex(NULL, false, NULL);
#else
        pthread_mutex_init(&this->m_mutex, NULL);
#endif
	}// end of queue()

	/* Distructure of class queue */
	template <class T>
	inline
	queue<T>::~queue()
	{
		this->clear_queue();
#ifdef _MSC_VER
        CloseHandle(this->m_mutex);
#else
        pthread_mutex_destroy(&this->m_mutex);
#endif
	}// end of ~queue

	/* Judge whether queue is empty */
	template <class T>
	inline
	bool
	queue<T>::is_empty()
	{
#ifdef _MSC_VER
        WaitForSingleObject(this->m_mutex, INFINITE);
#else
        pthread_mutex_lock(&this->m_mutex);
#endif
		int ret = (0 == this->m_length);
#ifdef _MSC_VER
        ReleaseMutex(this->m_mutex);
#else
        pthread_mutex_unlock(&this->m_mutex);
#endif
        return ret;
	}// end of is_empty

	/* Pop the queue head and receive */
	template <class T>
	inline
	int
	queue<T>::get_length()
	{
#ifdef _MSC_VER
        WaitForSingleObject(this->m_mutex, INFINITE);
#else
		pthread_mutex_lock(&this->m_mutex);
#endif
		int ret = this->m_length;
#ifdef _MSC_VER
        ReleaseMutex(this->m_mutex);
#else
		pthread_mutex_unlock(&this->m_mutex);
#endif
        return ret;
	}// end of get_length

	/* Pop the queue head and receive */
	template <class T>
	inline
	T
	queue<T>::pop()
	{
#ifdef _MSC_VER
        WaitForSingleObject(this->m_mutex, INFINITE);
#else
		pthread_mutex_lock(&this->m_mutex);
#endif
		assert(this->m_length);

		if (1 == this->m_length)
		{
			this->m_p_tail = nullptr;
		}// end of if
		queue<T>::queue_node *p_temp = this->m_p_head;
		T result = this->m_p_head->m_data;
		this->m_p_head = this->m_p_head->m_p_next;
		delete p_temp;

		this->m_length--;
#ifdef _MSC_VER
        ReleaseMutex(this->m_mutex);
#else
		pthread_mutex_unlock(&this->m_mutex);
#endif

		return result;
	}// end of pop

	/* Push new node to queue head */
	template <class T>
	inline
	void
	queue<T>::push(T data)
    {
#ifdef _MSC_VER
        WaitForSingleObject (this->m_mutex, INFINITE);
#else
		pthread_mutex_lock(&this->m_mutex);
#endif
		queue<T>::queue_node *p_new = new(std::nothrow) queue<T>::queue_node();
		if (nullptr == p_new)
		{
            exit(1);
		}// end of if

		 // copy data
		p_new->m_data = data;
		p_new->m_p_next = nullptr;

		if(0 == this->m_length) {
			this->m_p_head = p_new;
			this->m_p_tail = p_new;
		} else {
		this->m_p_tail->m_p_next = p_new;
		this->m_p_tail = p_new;
		}// end of if

		this->m_length++;
#ifdef _MSC_VER
        ReleaseMutex (this->m_mutex);
#else
		pthread_mutex_unlock(&this->m_mutex);
#endif
	}// end of push

	/* Delete all node */
	template <class T>
	inline
	void
	queue<T>::clear_queue()
    {
#ifdef _MSC_VER
        WaitForSingleObject (this->m_mutex, INFINITE);
#else
		pthread_mutex_lock(&this->m_mutex);
#endif
		queue<T>::queue_node *p_temp = this->m_p_head;
		while (nullptr != p_temp)
		{
			this->m_p_head = this->m_p_head->m_p_next;
			delete p_temp;

			p_temp = this->m_p_head;
		}// end of while

		this->m_p_tail = nullptr;
		this->m_p_head = nullptr;
		this->m_length = 0;
#ifdef _MSC_VER
        ReleaseMutex (this->m_mutex);
#else
		pthread_mutex_unlock(&this->m_mutex);
#endif
	}// end of clear_queue

}// end of namespace k

#endif //DY_BARRAGE_KQUEUE_H
