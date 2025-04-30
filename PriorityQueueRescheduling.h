#pragma once
#include "priQueue.h"
#include "Patient.h"
#include <cstdlib>
#include <ctime>
class PriorityQueueRescheduling : public priQueue<Patient*>
{
public:
	bool Reschedule(Patient*& Temp) {
		srand(time(0));
		int x;
		if (this->head == nullptr) {
			return false;
		}
		Patient* tempT;
		int p;
		if (this->count != 1) {
			x = ((rand()) % (this->count));
		}
		else x = 0;
		priNode<Patient*>* tempN = this->head;
		priNode<Patient*>* prevtempN = nullptr;
		int i = 0;
		if (x == 0) {
			this->head = this->head->getNext();
			tempT = tempN->getItem(p);
			Temp = tempT;
			x = ((rand()) % 2 + 1);
			tempN->getItem(p)->incPT(x);
			this->enqueue(tempN->getItem(p), -tempN->getItem(p)->getPT());
			this->count--;
			return true;
		}
		while (i != x && i < x) {
			prevtempN = tempN;
			tempN = tempN->getNext();
			i++;
		}
		if (tempN != NULL) {
			prevtempN->setNext(tempN->getNext());
			tempT = tempN->getItem(p);
			Temp = tempT;
			x = ((rand()) % 5 + 1);
			tempN->getItem(p)->incPT(x);
			this->enqueue(tempN->getItem(p), -tempN->getItem(p)->getPT());
			this->count--;
			return true;
		}
	}
};