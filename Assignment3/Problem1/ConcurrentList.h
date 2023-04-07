// Code based off of optimistic list from chapter 9.6 of "The Art of MultiProcessor Programming"
#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <mutex>

using namespace std;

class Node
{
public:
	int key;
	Node* next = nullptr;
	mutex l;

	Node(int key) { this->key = key; }
	void lock() { l.lock(); }
	void unlock() { l.unlock(); }
};

class ConcurrentList
{
private:
	Node* head = nullptr;
	recursive_mutex mutex;

public:

	ConcurrentList()
	{
		head = new Node(INT_MIN);
		head->next = new Node(INT_MAX);
	}

	bool add(int key)
	{
		while (true)
		{
			Node* pred = head;
			Node* curr = pred->next;
			while (curr->key <= key)
			{
				pred = curr;
				curr = curr->next;
			}
			pred->lock();
			curr->lock();
			if (validate(pred, curr))
			{
				if (curr->key == key)
				{
					pred->unlock();
					curr->unlock();
					return false;
				}
				else
				{
					Node* node = new Node(key);
					node->next = curr;
					pred->next = curr;
					pred->unlock();
					curr->unlock();
					return true;
				}
			}
			else
			{
				pred->unlock();
				curr->unlock();
				return false;
			}
		}
	}

	bool remove(int key)
	{
		while (true)
		{
			Node* pred = head;
			Node* curr = pred->next;
			while (curr->key < key)
			{
				pred = curr;
				curr = curr->next;
			}
			pred->lock();
			curr->lock();
			if (validate(pred, curr))
			{
				if (curr->key == key)
				{
					pred->next = curr->next;
					pred->unlock();
					curr->unlock();
					delete(curr);
					return true;
				}
				else
				{
					pred->unlock();
					curr->unlock();
					return false;
				}
			}
			else
			{
				pred->unlock();
				curr->unlock();
				return false;
			}
		}
	}

	bool contains(int key)
	{
		while (true)
		{
			Node* pred = head;
			Node* curr = pred->next;
			while (curr->key < key)
			{
				pred = curr;
				curr = curr->next;
			}
			pred->lock();
			curr->lock();
			if (validate(pred, curr))
			{
				pred->unlock();
				curr->unlock();
				return (curr->key == key);
			}
			else
			{
				pred->unlock();
				curr->unlock();
				return false;
			}
		}

		return false;
	}

	bool validate(Node* pred, Node* curr)
	{
		Node* node = head;
		while (node->key <= pred->key)
		{
			if (node == pred)
				return pred->next == curr;
			node = node->next;
		}
		return false;
	}
};