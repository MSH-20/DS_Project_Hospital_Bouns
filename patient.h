#pragma once
#include <iostream>
#include "Treatment.h"
#include "LinkedQueue.h"
using namespace std;
enum Patient_status { IDLE, ERLY, LATE, WAIT, SERV, FNSH };

class Patient
{
	static int PIDs;
	int PID;
	bool typeN; // true is normal  //false for recovering
	int PT;		// Appointment time
	int VT;		// Arrival time
	int FT;		// Finish time
	int TW;		// Total waiting time for this patient
	int TT;		// Total treatment time for this patient
	int XT;
	int IP;
	LinkedQueue<Treatment*> RequiredTreatements; // for the time needed for each treatment
	Patient_status PS;
	bool canceled;// Whether doing an accepted cancel	
	bool rescheduled;// Whether doing an accepted reschedule
public:

	Patient(char type, int p, int v) {
		PID = ++PIDs;
		if (type == 'N' || type == 'n') {
			typeN = true;
		}
		else if (type == 'R' || type == 'r') {
			typeN = false;
		}
		PT = p;
		VT = v;
		FT = 0;
		TW = 0;
		TT = 0;
		IP = 0;
		canceled = false;
		rescheduled = false;
		PS = IDLE;
	}

	//--------------------------------------------------------Bouns-------------------------------------------------------//

	void setIP()
	{
		IP = 1;
	}

	int getIP()
	{
		return IP;
	}

	bool isCanceled() const {
		return canceled;
	}
	bool isRescheduled() const {
		return rescheduled;
	}
	void setCanceled() {
		canceled = true;
	}
	void setRescheduled() {
		rescheduled = true;
	}
	void setFT(int t) {
		FT = t;
	}
	int getFT() const {
		return FT;
	}
	int getTT() const { // right logic
		return TT;
	}
	int getcurrentTimeStep(int timeStep) const {
		return timeStep - VT;
	}
	int getfinalTW() const { // wrong logic
		return FT - VT - TT;
	}
	void incPT(int k) {
		PT = PT + k;
	}
	int getPID() const {
		return PID;
	}
	void decTT() {
		TT = TT - XT;
	}
	int getPT() const {
		return PT;
	}
	Patient_status getPS() const {
		return PS;
	}
	int getVT() const {
		return VT;
	}
	void setE() {
		PS = ERLY;
	}
	void setL() {
		PS = LATE;
	}
	void setW() {
		PS = WAIT;
	}
	void setS() {
		PS = SERV;
	}
	void setF() {
		PS = FNSH;
	}
	bool istypeN() const {
		return typeN;
	}
	bool istypeR() const {
		return !typeN;
	}
	float getWaitTimeOfPatient() const {
		if (VT <= PT)
			return PT - VT;
		else
			return 1.5 * (VT - PT);
	}
	int getWaitOfFirstRequired() {
		Treatment* R;
		bool v = RequiredTreatements.peek(R);
		if (v) return R->getDuration();
		else return -1;
	}
	float getservingT() const {
		if (PT >= VT)
			return PT;
		else
			return PT + 0.5 * (VT - PT);
	}
	bool removeFirstRequired(Treatment*& R) {

		if (RequiredTreatements.isEmpty()) {
			return false;
		}
		else {
			RequiredTreatements.dequeue(R);
			return true;
		}
	}
	bool getFirstRequired(Treatment*& R) {

		if (RequiredTreatements.isEmpty()) {
			R = nullptr;
			return false;
		}
		else {
			RequiredTreatements.peek(R);
			return true;
		}
	}
	bool canCancel() {
		Treatment* r;
		RequiredTreatements.peek(r);
		if (RequiredTreatements.GetCount() == 1 && r->getTypet() == X) {
			return true;
		}
		else return false;
	}
	void addAttachedResource(Resources*& t) {
		Treatment* temp;
		RequiredTreatements.peek(temp);
		temp->setAssignedResource(t);
	}
	Resources*& getAttachedResource() {
		Treatment* temp;
		RequiredTreatements.peek(temp);
		return temp->getAssignedResource();
	}

	void removeAttachedResource() {
		Treatment* temp;
		RequiredTreatements.peek(temp);
		temp->resetAssignedResource();
	}
	void addTreat(int s, char t)
	{
		Treatment* temp;
		TT += s;
		switch (t)
		{
		case 'X': temp = new XTherapyTreatment(s); temp->setTypet(X); XT = s;  break;
		case 'E':  temp = new ETherapyTreatment(s); temp->setTypet(E); break;
		case 'U':  temp = new UTherapyTreatment(s); temp->setTypet(U); break;
		default:
			cout << "Invalid resource type character";
		}

		RequiredTreatements.enqueue(temp);
	}
	LinkedQueue<Treatment*> getRequiredTreatements()
	{
		return RequiredTreatements;
	}
	void removeTreat(Treatment* T)
	{
		if (RequiredTreatements.isEmpty())
		{
			return;
		}
		LinkedQueue<Treatment*> dummy;
		Treatment* T2;
		while (!RequiredTreatements.isEmpty())
		{
			RequiredTreatements.dequeue(T2);
			if (T2 != T)
			{
				dummy.enqueue(T2);
			}
		}
		while (!dummy.isEmpty())
		{
			dummy.dequeue(T2);
			RequiredTreatements.enqueue(T2);
		}
	}
	void addTreatFirst(Treatment* T)
	{
		LinkedQueue<Treatment*> temp = RequiredTreatements;
		Treatment* T2;
		while (!RequiredTreatements.isEmpty())
		{
			RequiredTreatements.dequeue(T2);
		}
		RequiredTreatements.enqueue(T);
		while (!temp.isEmpty())
		{
			temp.dequeue(T2);
			RequiredTreatements.enqueue(T2);
		}
	}


	void Print() const
	{
		if (PS == IDLE)
			cout << "P" << PID << "_" << VT;
		else if (PS == ERLY)
			cout << PID;
		else if (PS == LATE)
			cout << PID;
		else if (PS == WAIT)
			cout << PID;
		else if (PS == SERV) {
			Treatment* R;
			RequiredTreatements.peek(R);
			cout << "P" << PID << "_";
			R->getAssignedResource()->printForPatient();
		}
		else if (PS == FNSH)
			cout << PID;

	}

};
ostream& operator<<(ostream& os, const Patient* p);