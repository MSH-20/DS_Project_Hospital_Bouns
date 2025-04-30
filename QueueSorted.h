#pragma once
#include "LinkedQueue.h"
#include "Patient.h"

class QueueSorted :public LinkedQueue<Patient*> {
public:
	bool insertSorted(int timestep,Patient* item) {//to be continued when Patient done
		if (this->isEmpty()||item->getPS()==ERLY||this->backPtr->getItem()->getservingT()<= item->getservingT()) {
			this->enqueue(item);
			return true;
		}
		Node<Patient*>* ptr= this->frontPtr;
		Node<Patient*>* prev = nullptr;
		Node<Patient*>* newnode = new Node<Patient*>(item);
		bool f = false;
		while (ptr) {
			if (ptr->getItem()->getservingT()>item->getservingT()) {
				if (ptr == this->frontPtr) {
				newnode->setNext(this->frontPtr);
				this->frontPtr=newnode;
				this->count = this->count + 1;
				return true;
				}
			newnode->setNext(ptr);
			prev->setNext(newnode);
			this->count = this->count + 1;
			return true;
			}
			prev = ptr;
			ptr = ptr->getNext();
		}
	}
	int calcTreatmentLatency() {
		if (this->isEmpty()) {
			return 0;
		}
		else {
			Node<Patient*>* ptr = this->frontPtr;
			int sum = 0;
			while (ptr) {
				sum += ptr->getItem()->getWaitOfFirstRequired();
				ptr = ptr->getNext();
			}
			return sum;
		}
	}
};
