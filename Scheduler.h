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
		
		int x, dummy;
		Patient* P2 = nullptr;
		Patient* P3;
		Treatment* T;
		priQueue<Patient*> temp2;

		int y = In_Treatment.GetCount();
		bool found = false;

		if (y == 1)     // check the number of patients in In_Treatment
		{
			x = 0;
		}
		else
		{
			srand(time(0));
			x = ((rand()) % y) + 1;
		}


		if (x)        // based on count
		{
			priQueue<Patient*> temp = In_Treatment;
			while ((!found) && !(temp.isEmpty()))            //to find random patient
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
						if (y)
						{
							srand(time(0));
							x = ((rand()) % y) + 1;
						}
					}
					else
					{
						found = true;
					}
				}

			if (found)          // if the patient found
			{
				P2->getFirstRequired(T);
				if (T->getTypet() == E)
				{
					// edit
					P2->setW();
					P2->setIP();
					E_Interrupted_Patients.enqueue(P2, dummy);
				}
				if (T->getTypet() == U)
				{
					// edit
					P2->setW();
					P2->setIP();
					U_Interrupted_Patients.enqueue(P2, dummy);
				}
				T->setDuration(T->getDuration() - (timestep - T->getAssignmentTime()));     ///// case duration 1 and 0

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
		else         // in case In_Treatment has only one patient
		{
			In_Treatment.peek(P2, dummy);
			P2->getFirstRequired(T);
			Resources* R = P2->getAttachedResource();
			if (T->getTypet() == E)
			{
				P2->setW();
				P2->setIP();
				In_Treatment.dequeue(P2, dummy);
				E_Interrupted_Patients.enqueue(P2, dummy);
				T->setDuration(T->getDuration() - (timestep - T->getAssignmentTime()));
				R->decAttachedPatientsCount();
				R->setFT();
				E_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
				P2->removeAttachedResource();
			}
			else if (T->getTypet() == U)
			{
				P2->setW();
				P2->setIP();
				In_Treatment.dequeue(P2, dummy);
				U_Interrupted_Patients.enqueue(P2, dummy);
				T->setDuration(T->getDuration() - (timestep - T->getAssignmentTime()));
				R->decAttachedPatientsCount();
				R->setFT();
				U_Maintenance_Devices.enqueue(R, -timestep - R->getMaintenance_Time());
				P2->removeAttachedResource();
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

	void ReadInput(string filename)
 	{

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

		string filename;
		filename = ui.getFileName(mode);
		ReadInput(filename);

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
			srand(time(0));
			X = rand() % 101;
			if (X < PBusyFail)
			{
				if (!In_Treatment.isEmpty())
				{
					Busy_Failure(timestep);
				}
			}


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
					P2->removeAttachedResource();
				}
				else if (R->getType() == E) {
					R->decAttachedPatientsCount();
					E_Devices.enqueue(R);
					P2->removeAttachedResource();
				}
				else if (R->getType() == U) {
					R->decAttachedPatientsCount();
					U_Devices.enqueue(R);
					P2->removeAttachedResource();
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
 		filename = ui.outputfile();
  		MakeOutFile(filename, timestep);

	}

	void MakeOutFile(string filename, int timestep)
	{

  		ofstream outFile(filename);
  		if (!outFile) {
  			cout << "Error creating file!" << endl;
  			return;
  		} 

        outFile << "PID  PType PT   VT   FT   WT   TT   Cancel Resc" << endl;
        float totalnumber, Nnumber, Rnumber, Wall, WR, WN, Tall, TN, TR, accCP, accRP, IP, FDF, E_FDF, U_FDF;
        float Pearly, Plate, latePen;

        totalnumber =  Nnumber = Rnumber = Wall = WR = WN = Tall = TN = TR = accCP = accRP = Pearly = Plate = latePen = IP = E_FDF = U_FDF = FDF = 0;

			while (!Finished_Patients.isEmpty())
			{
				Patient* temp;
				Finished_Patients.pop(temp);

				//--------------------------------------------------------Bouns-------------------------------------------------------//
				IP += temp->getIP();
				//--------------------------------------------------------Bouns-------------------------------------------------------//


				if (temp->getPID() < 10)
				{
					outFile << "P" << temp->getPID() << "   ";
				}
				else if (temp->getPID() < 100)
				{
					outFile << "P" << temp->getPID() << "  ";
				}
				else
				{
					outFile << "P" << temp->getPID() << " ";
				}
				if (temp->istypeN())
					outFile << "N" << "     ";
				if (temp->istypeR())
					outFile << "R" << "     ";

				if (temp->getPT() < 10)
				{
					outFile << temp->getPT() << "    ";
				}
				else if (temp->getPT() < 100)
				{
					outFile << temp->getPT() << "   ";
				}
				else if (temp->getPT() < 1000)
				{
					outFile << temp->getPT() << "  ";
				}
				else
				{
					outFile << temp->getPT() << " ";
				}

				if (temp->getVT() < 10)
				{
					outFile << temp->getVT() << "    ";
				}
				else if (temp->getVT() < 100)
				{
					outFile << temp->getVT() << "   ";
				}
				else if (temp->getVT() < 1000)
				{
					outFile << temp->getVT() << "  ";
				}
				else
				{
					outFile << temp->getVT() << " ";
				}

				if (temp->getFT() < 10)
				{
					outFile << temp->getFT() << "    ";
				}
				else if (temp->getFT() < 100)
				{
					outFile << temp->getFT() << "   ";
				}
				else if (temp->getFT() < 1000)
				{
					outFile << temp->getFT() << "  ";
				}
				else
				{
					outFile << temp->getFT() << " ";
				}

				if (temp->getfinalTW() < 10)
				{
					outFile << temp->getfinalTW() << "    ";
				}
				else if (temp->getfinalTW() < 100)
				{
					outFile << temp->getfinalTW() << "   ";
				}
				else if (temp->getfinalTW() < 1000)
				{
					outFile << temp->getfinalTW() << "  ";
				}
				else
				{
					outFile << temp->getfinalTW() << " ";
				}

				if (temp->getTT() < 10)
				{
					outFile << temp->getTT() << "    ";
				}
				else if (temp->getTT() < 100)
				{
					outFile << temp->getTT() << "   ";
				}
				else if (temp->getTT() < 1000)
				{
					outFile << temp->getTT() << "  ";
				}
				else
				{
					outFile << temp->getTT() << " ";
				}

				if (temp->isCanceled())
				{
					outFile << "T      ";
					++accCP;
				}
				else
				{
					outFile << "F      ";
				}
				if (temp->isRescheduled())
				{
					outFile << "T\n";
					++accRP;
				}
				else
				{
					outFile << "F\n";
				}

				++totalnumber;
				Wall += temp->getfinalTW();
				Tall += temp->getTT();
				if (temp->istypeN())
				{
					++Nnumber;
					WN += temp->getfinalTW();
					TN += temp->getTT();
				}
				else
				{
					++Rnumber;
					WR += temp->getfinalTW();
					TR += temp->getTT();
				}

				if (temp->isEarly())
					++Pearly;
				else if (temp->isLate())
				{
					++Plate;
					latePen += temp->getWaitTimeOfPatient();
				}


			}
		

        //--------------------------------------------------------Bouns-------------------------------------------------------//
        int E_Count = E_Devices.GetCount();
        int U_Count = U_Devices.GetCount();

        while (!E_Devices.isEmpty())
        {
            Resources* R;
            if (E_Devices.dequeue(R))
                E_FDF += R->getFT();
        }

        while (!U_Devices.isEmpty())
        {
            Resources* R;
            if (U_Devices.dequeue(R))
                U_FDF += R->getFT();
        }
        //--------------------------------------------------------Bouns-------------------------------------------------------//

		Wall /= totalnumber;
		Tall /= totalnumber;

		WN /= Nnumber;
		TN /= Nnumber;

		WR /= Rnumber;
		TR /= Rnumber;

		accCP = accCP / totalnumber * 100;
		accRP = accRP / totalnumber * 100;

		latePen /= Plate;

		Pearly = Pearly / totalnumber * 100;
		Plate  = Plate / totalnumber * 100;

		FDF = ((E_FDF + U_FDF + (E_Maintenance_Devices.GetCount() + U_Maintenance_Devices.GetCount())) / (E_Count + U_Count + E_Maintenance_Devices.GetCount() + U_Maintenance_Devices.GetCount())) * 100;

		outFile << "\nTotal number of timesteps = " << timestep << endl;
		outFile << "Total number of All, N, R = " << totalnumber << ", " << Nnumber << ", " << Rnumber << endl;
		outFile << "Average total waiting time of All, N, R = " << Wall << ", " << WN << ", " << WR << endl;   // wrong logic
		outFile << "Average total treatment time  of All, N, R = " << Tall << ", " << TN << ", " << TR << endl; // wrong logic
		outFile << "Percentage of patients that accepted cancellation (%) = " << accCP << " %\n";
		outFile << "Percentage of patients that accepted rescheduling (%) = " << accRP << " %\n";

		        //--------------------------------------------------------Bouns-------------------------------------------------------//
				outFile << "Percentage of free devices that failed (%) = " << FDF << " %\n";
				outFile << "Percentage of Interrupted patients (%) = " << ((IP/totalnumber) * 100) << " %\n";
				//--------------------------------------------------------Bouns-------------------------------------------------------//

		outFile << "Percentage of early patients (%) = " << Pearly << " %\n";
		outFile << "Percentage of late patients (%) = " << Plate << " %\n";
		outFile << "Average late penalty = " << latePen << " timestep(s)\n";

		outFile.close();

		cout << "\nFile created at: " << filename << endl << endl;

		return;
    }

	~Scheduler() {}
};