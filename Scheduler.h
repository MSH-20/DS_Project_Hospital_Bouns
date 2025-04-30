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

	//--------------------------------Bouns----------------------------------------//
	int PFreeFail;
	int PBusyFail;
	int Maintenance_Time;
	priQueue<Resources*>  E_Maintenance_Devices;
	priQueue<Resources*>  U_Maintenance_Devices;

	priQueue<Patient*> E_Interrupted_Patients;
	priQueue<Patient*> U_Interrupted_Patients;
	//--------------------------------Bouns----------------------------------------//


public:

	bool Free_Failure(Resources* R, int timestep)
	{
		if (R)
		{
			srand(time(0));
			int X;
			X = rand() % 101;
			if (X < PFreeFail)
			{
				if (R->getType() == E)
				{
					R->setFT();
					E_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
					return false;
				}
				if (R->getType() == U)
				{
					R->setFT();
					U_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
					return false;
				}

			}
		}
		
		return true;
	}



	void Busy_Failure(int timestep)
	{
		srand(time(0));
		int x, dummy;
		Patient* P2 = nullptr;
		Patient* P3;
		Treatment* T;
		priQueue<Patient*> temp2;


		if (!In_Treatment.isEmpty())
		{
			int y = In_Treatment.GetCount();
			bool found = false;
			if (y == 1)
			{
				x = 0;
			}
			else
			{
				x = ((rand()) % y) + 1;
			}
			if (x)
			{
				priQueue<Patient*> temp = In_Treatment;
				while (!found && !temp.isEmpty())
				{
					for (int i = 0; i < (x-1); i++)
					{
						temp.dequeue(P2, dummy);
						temp2.enqueue(P2, dummy);
					}
					temp.dequeue(P2, dummy);
					if (P2->getAttachedResource()->getType() == Type::X)
					{
						while (!temp.isEmpty())
						{
							temp.dequeue(P2, dummy);
							temp2.enqueue(P2, dummy);
						}
						while (!temp2.isEmpty())
						{
							temp2.dequeue(P2, dummy);
							temp.enqueue(P2, dummy);
						}
						y--;
						if(y)
						x = ((rand()) % y) + 1;
					}
					else
					{
						found = true;
					}
				}

				if (found)
				{
					P2->getFirstRequired(T);
					if (T->getTypet() == E)
					{
						// edit
						P2->setW();
						
						E_Interrupted_Patients.enqueue(P2, dummy);
					}
					if (T->getTypet() == U)
					{
						// edit
						P2->setW();
						
						U_Interrupted_Patients.enqueue(P2, dummy);
					}
					T->setDuration(T->getDuration() - (timestep - T->getAssignmentTime()));

					Resources* R = P2->getAttachedResource();
					R->decAttachedPatientsCount();

					if (R->getType() == E)
					{
						R->setFT();
						E_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
						P2->removeAttachedResource();
						//R = nullptr;
					}
					if (R->getType() == U)
					{
						R->setFT();
						U_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
						P2->removeAttachedResource();
						//R = nullptr;
					}
					//P2->addAttachedResource(nullptr); to remove the attached device from it

					//we now remove the patient from the In_Treatment list
					
					found = false;
					while (!temp.isEmpty())
					{
						temp.dequeue(P3, dummy);
					}
					while ((!In_Treatment.isEmpty()) && (!found))
					{
						if (In_Treatment.dequeue(P3, dummy))
						{
							if (P3->getPID() == P2->getPID())
							{
								found = true;
							}
							else
							{
								temp.enqueue(P3, dummy);
							}
						}

					}
					while (!In_Treatment.isEmpty())
					{
						In_Treatment.dequeue(P3, dummy);
						temp.enqueue(P3, dummy);
					}
					while (!temp.isEmpty())
					{
						temp.dequeue(P3, dummy);
						In_Treatment.enqueue(P3, dummy);
					}
				}
			}
			else
			{
				In_Treatment.peek(P2, dummy);
				if (P2->getAttachedResource()->getType() == E)
				{
					In_Treatment.dequeue(P2, dummy);
					E_Interrupted_Patients.enqueue(P2, dummy);
				}
				if (P2->getAttachedResource()->getType() == U)
				{
					In_Treatment.dequeue(P2, dummy);
					U_Interrupted_Patients.enqueue(P2, dummy);
				}
			}


		}
	}


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

		//--------------------------------------------------------Bouns-------------------------------------------------------//
		int eCount, uCount, xCount;
		int eMaintenance_Time, uMaintenance_Time;
		inFile >> eCount >> eMaintenance_Time;
		inFile >> uCount >> uMaintenance_Time;
		inFile >> xCount;

		for (int i = 0; i < eCount; i++)
			E_Devices.enqueue(new ETherapy(eMaintenance_Time));

		for (int i = 0; i < uCount; i++)
			U_Devices.enqueue(new UTherapy(uMaintenance_Time));

		for (int i = 0; i < xCount; ++i)
		{
			int cap;
			inFile >> cap;

			Gym* g = new Gym();
			g->setCap(cap);
			X_Devices.enqueue(g);
		}

		inFile >> PCancel >> PReschedule >> PFreeFail >> PBusyFail;

		//--------------------------------------------------------Bouns-------------------------------------------------------//

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

		//--------------------------------------------------------Bouns-------------------------------------------------------//
		Patient* P3;
		Resources* R2;
		//--------------------------------------------------------Bouns-------------------------------------------------------//

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

			srand(time(0));
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

			//srand(time(0));
			//X = rand() % 101;
			//if (X < PReschedule)//reschedule case
			//{
			//	if (Early_Patients.Reschedule(P)) {
			//		P->setRescheduled();
			//	}
			//}


			//----------------------------------------------------Bouns-------------------------------------------------------//
			//srand(time(0));
			//X = rand() % 101;
			//if (X < PBusyFail)//reschedule case
			//{
			//	if (In_Treatment.peek(P,dummy))
			//	{
			//		Busy_Failure(timestep);
			//	}
			//}


			while (E_Maintenance_Devices.peek(R2, dummy) && (-dummy) <= timestep)
			{
				E_Maintenance_Devices.dequeue(R2, dummy);
				if (R2)
					E_Devices.enqueue(R2);
			}

			while (U_Maintenance_Devices.peek(R2, dummy) && (-dummy) <= timestep)
			{
				U_Maintenance_Devices.dequeue(R2, dummy);
				if (R2)
					U_Devices.enqueue(R2);
			}

			//--------------------------------------------------------Bouns-------------------------------------------------------//

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


			//--------------------------------------------------------Bouns-------------------------------------------------------//


			while (E_Devices.peek(R) && E_Interrupted_Patients.peek(P3, dummy)) { //going out of E_Interrupted_Patients to In_treatment
				P3->setS();
				E_Devices.dequeue(R);
				//bool x = false;
				//while ((!x) && (!Free_Failure(R, timestep)))
				//{
				//	if (!E_Devices.dequeue(R))
				//	{
				//		x = true;
				//	}
				//}
				if (!Free_Failure(R, timestep))
				{
					if (U_Devices.dequeue(R))
					{
						R->incAttachedPatientsCount();
						P3->addAttachedResource(R);
						P3->getFirstRequired(T);
						if (T)
						{
							In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
							T->setAssignmentTime(timestep);
						}
						E_Interrupted_Patients.dequeue(P3, dummy);
					}
					else
					{
						P3->setW();
					}
				}
				else
				{
					R->incAttachedPatientsCount();
					P3->addAttachedResource(R);
					P3->getFirstRequired(T);
					if (T)
					{
						In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
						T->setAssignmentTime(timestep);
					}
					E_Interrupted_Patients.dequeue(P3, dummy);
				}
			}

			while (U_Devices.peek(R) && U_Interrupted_Patients.peek(P3, dummy)) {//going out of U_Interrupted_Patients to In_treatment
				P3->setS();
				U_Devices.dequeue(R);
				//bool x = false;
				//while ((!x) && (!Free_Failure(R, timestep)))
				//{
				//	if (!U_Devices.dequeue(R))
				//	{
				//		x = true;
				//	}
				//}
				if (!Free_Failure(R, timestep))
				{
					if (U_Devices.dequeue(R))
					{
						R->incAttachedPatientsCount();
						P3->addAttachedResource(R);
						P3->getFirstRequired(T);
						if (T)
						{
							In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
							T->setAssignmentTime(timestep);
						}
						U_Interrupted_Patients.dequeue(P3, dummy);
					}
					else
					{
						P3->setW();
					}
				}
				else
				{
					R->incAttachedPatientsCount();
					P3->addAttachedResource(R);
					P3->getFirstRequired(T);
					if (T)
					{
						In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
						T->setAssignmentTime(timestep);
					}
					U_Interrupted_Patients.dequeue(P3, dummy);
				}
			}


			while (E_Devices.peek(R) && E_Waiting_Patients.peek(P3)) { //going out of E_waiting to In_treatment
				P3->setS();
				E_Devices.dequeue(R);
				//bool x = false;
				//while ((!x) && (!Free_Failure(R, timestep)))
				//{
				//	if (!E_Devices.dequeue(R))
				//	{
				//		x = true;
				//	}
				//}
				if (!Free_Failure(R, timestep))
				{
					if (E_Devices.dequeue(R))
					{
						R->incAttachedPatientsCount();
						P3->addAttachedResource(R);
						P3->getFirstRequired(T);
						if (T)
						{
							In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
							T->setAssignmentTime(timestep);
						}
						E_Waiting_Patients.dequeue(P3);
					}
					else
					{
						P3->setW();
					}
				}
				else
				{
					R->incAttachedPatientsCount();
					P3->addAttachedResource(R);
					P3->getFirstRequired(T);
					if (T)
					{
						In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
						T->setAssignmentTime(timestep);
					}
					E_Waiting_Patients.dequeue(P3);
				}

			}




			while (U_Devices.peek(R) && U_Waiting_Patients.peek(P3)) {//going out of U_waiting to In_treatment
				P3->setS();
				U_Devices.dequeue(R);
				//bool x = false;
				//while ((!x) && (!Free_Failure(R, timestep)))
				//{
				//	if (!U_Devices.dequeue(R))
				//	{
				//		x = true;
				//	}
				//}
				//if (!x)
				if (!Free_Failure(R, timestep))
				{
					if (U_Devices.dequeue(R))
					{
						R->incAttachedPatientsCount();
						P3->addAttachedResource(R);
						P3->getFirstRequired(T);
						if (T)
						{
							In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
							T->setAssignmentTime(timestep);
						}
						U_Waiting_Patients.dequeue(P3);
					}
					else
					{
						P3->setW();
					}
				}
				else
				{
					R->incAttachedPatientsCount();
					P3->addAttachedResource(R);
					P3->getFirstRequired(T);
					if (T)
					{
						In_Treatment.enqueue(P3, -timestep - (T->getDuration()));
						T->setAssignmentTime(timestep);
					}
					U_Waiting_Patients.dequeue(P3);
				}

			}


			//--------------------------------------------------------Bouns-------------------------------------------------------//



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
				T->setAssignmentTime(timestep);
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
				ui.printConsle(timestep, E_Maintenance_Devices, U_Maintenance_Devices, E_Interrupted_Patients, U_Interrupted_Patients, All_Patients, X_Devices, E_Devices, U_Devices, Early_Patients, Late_Patients, Finished_Patients, In_Treatment, U_Waiting_Patients, E_Waiting_Patients, X_Waiting_Patients);
			}
			timestep++;
			Finished_PatientsCount = Finished_Patients.GetCount();
		}
		ui.outputfile(Finished_Patients, E_Devices, U_Devices, timestep);

	}

	~Scheduler() {};
};