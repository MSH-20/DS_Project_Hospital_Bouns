#pragma once
#include "QueueSorted.h"
#include <cstdlib>
#include <ctime>
class QueueSortedWithCancel : public QueueSorted {
public:
	bool cancel(Patient*& T) {
		{
			srand(time(0));
			int x;
			if (this->frontPtr == nullptr) {
				return false;
			}
			Patient* t;
			if (this->count != 1) {
				x = ((rand()) % (this->count));
			}
			else x = 0;
			Node<Patient*>* temp = this->frontPtr;
			Node<Patient*>* prevtemp = nullptr;
			int i = 0;
			if (x == 0 && temp->getItem()->canCancel()) {
				this->frontPtr = this->frontPtr->getNext();
				T = temp->getItem();
				this->count = this->count - 1;
				return true;
			}
			while (i != x && i < x) {
				prevtemp = temp;
				temp = temp->getNext();
				i++;
			}
			if (temp->getNext() == NULL && temp->getItem()->canCancel())
			{
				prevtemp->setNext(temp->getNext());
				this->backPtr = prevtemp;
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			if (temp != NULL && temp->getItem()->canCancel()) {
				prevtemp->setNext(temp->getNext());
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			while (temp && !temp->getItem()->canCancel()) {
				prevtemp = temp;
				temp = temp->getNext();
				i++;
				if (i >= this->count) {
					i = 0;
					temp = this->frontPtr;
				}
				if (i == x) {
					return false;
				}
			}
			if (i == 0) {
				this->frontPtr = this->frontPtr->getNext();
				T = temp->getItem();
				this->count = this->count - 1;
				return true;
			}
			if (i ==this->count-1)
			{
				prevtemp->setNext(temp->getNext());
				this->backPtr = prevtemp;
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			if (temp != NULL) {
				prevtemp->setNext(temp->getNext());
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			return false;
		}
	}
	bool cancel2(Patient*& T) {
		{
			srand(time(0));
			int x;
			if (this->frontPtr == nullptr) {
				return false;
			}
			Patient* t;
			if (this->count != 1) {
				x = ((rand()) % (this->count));
			}
			else x = 0;
			Node<Patient*>* temp = this->frontPtr;
			Node<Patient*>* prevtemp = nullptr;
			int i = 0;
			if (x == 0 && temp->getItem()->canCancel()) {
				this->frontPtr = this->frontPtr->getNext();
				T = temp->getItem();
				this->count = this->count - 1;
				return true;
			}

			while (i != x && i < x) {
				prevtemp = temp;
				temp = temp->getNext();
				i++;
			}
			if (temp->getNext() == NULL && temp->getItem()->canCancel())
			{
				prevtemp->setNext(temp->getNext());
				this->backPtr = prevtemp;
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			if (temp != NULL && temp->getItem()->canCancel()) {
				prevtemp->setNext(temp->getNext());
				T = (temp->getItem());
				this->count = this->count - 1;
				return true;
			}
			return false;
		}
	}
};
