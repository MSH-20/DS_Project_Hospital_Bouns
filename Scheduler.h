#pragma once
#include <iostream>

#include "LinkedQueue.h"
#include "priQueue.h"
#include "patient.h"
#include "ArrayStack.h"
#include "PriorityQueueRescheduling.h"
#include "QueueSorted.h"
#include "QueueSortedWithCancel.h"
#include "Resources.h"
#include <fstream>
#include <string>
#include "UI.h"


using namespace std;


class Scheduler
{
private:
	LinkedQueue<Patient*>  All_Patients;
	PriorityQueueRescheduling Early_Patients;
	priQueue<Patient*>  Late_Patients;
	LinkedQueue<Resources*>  E_Devices;
	LinkedQueue<Resources*>  U_Devices;
	LinkedQueue<Resources*>  X_Devices;
	priQueue<Patient*>  In_Treatment;
	ArrayStack<Patient*>  Finished_Patients;
	QueueSorted U_Waiting_Patients;
	QueueSorted E_Waiting_Patients;
	QueueSortedWithCancel X_Waiting_Patients;
	UI ui;
	int PCancel;
	int PReschedule;
public:
	// Functions here
	int getCount_X()
	{
		return X_Devices.GetCount();
	}
	int getCount_E()
	{
		return E_Devices.GetCount();
	}
	int getCount_U()
	{
		return U_Devices.GetCount();
	}

	void AddToWait_U(int timestep, Patient* P)
	{
		P->setW();
		U_Waiting_Patients.insertSorted(-timestep, P);
	}

	void AddToWait_E(int timestep, Patient* P)
	{
		P->setW();
		E_Waiting_Patients.insertSorted(-timestep, P);
	}

	void AddToWait_X(int timestep, Patient* P)
	{
		P->setW();
		X_Waiting_Patients.insertSorted(-timestep, P);
	}

	void ReadInput(char& mode)
	{

		string filename = ui.getFileName(mode);

		filename = "input files\\" + filename + ".txt";

		ifstream inFile(filename);
		if (!inFile.is_open())
		{
			cout << "Error: could not open input file.\n";
			return;
		}

		int eCount, uCount, xCount;
		inFile >> eCount >> uCount >> xCount;

		for (int i = 0; i < eCount; i++)
			E_Devices.enqueue(new ETherapy());

		for (int i = 0; i < uCount; i++)
			U_Devices.enqueue(new UTherapy());

		for (int i = 0; i < xCount; ++i)
		{
			int cap;
			inFile >> cap;

			Gym* g = new Gym();
			g->setCap(cap);
			X_Devices.enqueue(g);
		}

		inFile >> PCancel >> PReschedule;

		int patientCount;
		inFile >> patientCount;

		for (int i = 0; i < patientCount; ++i)
		{
			char type;
			int pt, vt;
			inFile >> type >> pt >> vt;

			Patient* p = new Patient(type, pt, vt);

			int numberofTreatments;
			inFile >> numberofTreatments;

			for (int i = 0; i < numberofTreatments; ++i)
			{
				char type0fTreatments;
				int numberofSteps;
				inFile >> type0fTreatments >> numberofSteps;
				//  Required Treatements on progress......
				p->addTreat(numberofSteps, type0fTreatments);
			}

			All_Patients.enqueue(p);
		}

		inFile.close();
		cin.ignore();
	}


	void RP(int timestep, Patient*& P, Treatment*& T)
	{
		LinkedQueue<Treatment*> dummy = P->getRequiredTreatements();
		LinkedQueue<Treatment*> dummy2 = dummy;
		Treatment* T2;
		int count = dummy.GetCount();

		if (count == 1)
		{
			T->MoveToWait(this, P, timestep);
			return;
		}
		int x = 0, y = 0, z = 0;
		while (!dummy2.isEmpty())
		{
			dummy2.dequeue(T);
			if (T->getTypet() == E)
			{
				x = E_Waiting_Patients.calcTreatmentLatency();
			}
			if (T->getTypet() == U)
			{
				y = U_Waiting_Patients.calcTreatmentLatency();
			}
			if (T->getTypet() == Type::X)
			{
				z = X_Waiting_Patients.calcTreatmentLatency();
			}
		}
		if (count == 2)
		{
			while (!dummy.isEmpty())
			{
				dummy.dequeue(T);
				if (T->getTypet() == E && (x <= y || x <= z))
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
				else if (T->getTypet() == U && (y <= x || y <= z))
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
				else if (T->getTypet() == Type::X && (z <= y || z <= x))
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
			}
		}
		else if (count == 3)
		{
			while (!dummy.isEmpty())
			{
				dummy.dequeue(T);
				if (T->getTypet() == E && x <= y && x <= z)
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
				else if (T->getTypet() == U && y <= x && y <= z)
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
				else if (T->getTypet() == X && z <= y && z <= x)
				{
					P->setW();
					P->removeTreat(T);
					P->addTreatFirst(T);
					T->MoveToWait(this, P, timestep);
					return;
				}
			}
		}
	}

	void simulation()
	{
		char mode;
		ReadInput(mode);
		int timestep = 0;
		int c = All_Patients.GetCount();
		Patient* P;
		Patient* P2;
		Resources* R;
		Treatment* T;

		int Finished_PatientsCount = Finished_Patients.GetCount();

		while (c != Finished_PatientsCount && c > Finished_PatientsCount)
		{
			P = nullptr;
			P2 = nullptr;
			R = nullptr;
			T = nullptr;
			int dummy;



			while (All_Patients.peek(P) && P->getVT() == timestep) {//entering early or late list case
				if (P->getVT() == timestep)
				{
					All_Patients.dequeue(P);
					if (P->getVT() <= P->getPT())
					{
						P->setE();
						Early_Patients.enqueue(P, -P->getservingT());
					}
					else if (P->getVT() > P->getPT())
					{
						P->setL();
						Late_Patients.enqueue(P, -P->getservingT());
					}
				}
			}


			int X = rand() % 101;
			if (X < PCancel)//cancel case
			{
				if (X_Waiting_Patients.cancel2(P))
				{
					P->setF();
					P->setCanceled();
					P->decTT();
					P->setFT(timestep);
					Finished_Patients.push(P);
				}
			}


			X = rand() % 101;
			if (X < PReschedule)//reschedule case
			{
				if (Early_Patients.Reschedule(P)) {
					P->setRescheduled();
				}
			}


			while (Early_Patients.peek(P, dummy) && P->getPT() <= timestep) {//going out of early to waiting
				Early_Patients.dequeue(P, dummy);
				if (P->istypeN()) {
					P->getFirstRequired(T);
					T->MoveToWait(this, P, -timestep);
				}
				else if (P->istypeR()) {// to be edited
					P->getFirstRequired(T);
					if (T) {
						P->setW();
						RP(timestep, P, T);
					}
				}
			}



			while (Late_Patients.peek(P, dummy) && (P->getVT() + 1.5 * (P->getVT() - P->getPT())) <= timestep) {//going out of late to waiting
				Late_Patients.dequeue(P, dummy);
				if (P->istypeN()) {
					P->getFirstRequired(T);
					T->MoveToWait(this, P, timestep);
				}
				else if (P->istypeR()) {// to be edited
					P->getFirstRequired(T);
					if (T) {
						P->setW();
						RP(timestep - (P->getVT() - P->getPT()), P, T);
					}
				}
			}


			while (E_Waiting_Patients.peek(P2) && P2->getFirstRequired(T) && T->CanAssign(this)) { //going out of E_waiting to In_treatment
				E_Waiting_Patients.dequeue(P2);
				P2->setS();
				E_Devices.dequeue(R);
				R->incAttachedPatientsCount();
				P2->addAttachedResource(R);
				In_Treatment.enqueue(P2, -timestep - (T->getDuration()));
			}


			while (U_Waiting_Patients.peek(P2) && P2->getFirstRequired(T) && T->CanAssign(this)) {//going out of U_waiting to In_treatment
				U_Waiting_Patients.dequeue(P2);
				P2->setS();
				U_Devices.dequeue(R);
				R->incAttachedPatientsCount();
				P2->addAttachedResource(R);
				In_Treatment.enqueue(P2, -timestep - (T->getDuration()));
			}


			while (X_Waiting_Patients.peek(P2) && P2->getFirstRequired(T) && T->CanAssign(this)) {//going out of X_waiting to In_treatment
				X_Waiting_Patients.dequeue(P2);
				P2->setS();
				X_Devices.peek(R);
				R->incAttachedPatientsCount();
				if (R->isFull()) {
					X_Devices.dequeue(R);
				}
				P2->addAttachedResource(R);
				In_Treatment.enqueue(P2, -timestep - (T->getDuration()));
			}



			while (In_Treatment.peek(P, dummy) && (-dummy) <= timestep) { //going out of In_treatment to Finish or Waiting List
				In_Treatment.dequeue(P2, dummy);
				R = P2->getAttachedResource();
				if (R->getType() == Type::X) {
					if (R->isFull()) {
						X_Devices.enqueue(R);
					}
					R->decAttachedPatientsCount();
				}
				else if (R->getType() == E) {
					R->decAttachedPatientsCount();
					E_Devices.enqueue(R);
				}
				else if (R->getType() == U) {
					R->decAttachedPatientsCount();
					U_Devices.enqueue(R);
				}
				if (P->istypeN()) {
					P->removeFirstRequired(T);
					P->getFirstRequired(T);
					if (T) {
						T->MoveToWait(this, P, timestep);
					}
					else {
						P->setF();
						P->setFT(timestep);
						Finished_Patients.push(P);
					}
				}
				else if (P->istypeR()) {// to be edited
					P->removeFirstRequired(T);
					P->getFirstRequired(T);
					if (T) {
						P->setW();
						RP(timestep, P, T);
					}
					else {
						P->setF();
						P->setFT(timestep);
						Finished_Patients.push(P);
					}
				}
			}

			if (mode != 's')
			{
				//system("cls"); //if we wanted to clear the console
				ui.printConsle(timestep, All_Patients, X_Devices, E_Devices, U_Devices, Early_Patients, Late_Patients, Finished_Patients, In_Treatment, U_Waiting_Patients, E_Waiting_Patients, X_Waiting_Patients);
			}
			timestep++;
			Finished_PatientsCount = Finished_Patients.GetCount();
		}
		ui.outputfile(Finished_Patients, timestep);

	}

	~Scheduler() {};
};