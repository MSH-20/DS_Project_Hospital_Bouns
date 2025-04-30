#pragma once
#include "priNode.h"
#include "Patient.h"

//This class impelements the priority queue as a sorted list (Linked List)
//The item with highest priority is at the front of the queue
template <typename T>
class priQueue
{
protected:
    priNode<T>* head;
    int count;
public:
    priQueue() : head(nullptr), count(0) {}

    ~priQueue() {
        T tmp;
        int p;
        while (dequeue(tmp, p));
    }

    //insert the new node in its correct position according to its priority
    void enqueue(const T& data, int priority) {
        priNode<T>* newNode = new priNode<T>(data, priority);

        if (head == nullptr || priority > head->getPri()) {

            newNode->setNext(head);
            head = newNode;
            count = count + 1;
            return;
        }

        priNode<T>* current = head;
        while (current && current->getNext() && priority <= current->getNext()->getPri()) {
            current = current->getNext();
        }
        newNode->setNext(current->getNext());
        current->setNext(newNode);
        count = count + 1;
    }

    bool dequeue(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        priNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        count = count - 1;
        return true;
    }

    bool peek(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        //pri = head->getPri();
        return true;
    }


    int GetCount() const
    {
        return count;
    }

    void PrintPriQueue() const
    {
        int p;
        priNode<T>* tempPtr = this->head;
        while (tempPtr)
        {

            cout << tempPtr->getItem(p);
            if (tempPtr->getNext()) {
                cout << ", ";
            }
            tempPtr = tempPtr->getNext();

        }
        cout << endl;
    }
    bool isEmpty() const {
        return head == nullptr;
    }
};
