#pragma once


#include "LinkedQueue.h"
#include "priQueue.h"
#include "patient.h"
#include "ArrayStack.h"
#include "PriorityQueueRescheduling.h"
#include "QueueSorted.h"
#include "QueueSortedWithCancel.h"
#include "Resources.h"
	
struct UI
{
public:
	string getFileName(char& mode)
	{
		cout << "Enter file name (without extension): ";
		string name;
		cin >> name;
        cout << "what to mode u want: ";
        cin >> mode;
        return name;
	}

    void printConsle(int timestep,
        const priQueue<Resources*>& E_Maintenance_Devices,
        const priQueue<Resources*>& U_Maintenance_Devices,
        const priQueue<Patient*>& E_Interrupted_Patients,
        const priQueue<Patient*>& U_Interrupted_Patients,
        const LinkedQueue<Patient*>& Patients,
        const LinkedQueue<Resources*>& X_Deivces,
        const LinkedQueue<Resources*>& E_Devices,
        const LinkedQueue<Resources*>& U_Deivces,
        const PriorityQueueRescheduling& Early_Patients,
        const priQueue<Patient*>& Late_Patients,
        const ArrayStack<Patient*>& Finished_Patients,
        const priQueue<Patient*>& In_Treatment,
        const QueueSorted& U_Waiting_Patients,
        const QueueSorted& E_Waiting_Patients,
        const QueueSortedWithCancel& X_Waiting_Patients) const
    {
        cout << "Current Timestep: " << timestep << endl;
        cout << "================= ALL List =================" << endl;

        int patientCount = Patients.GetCount();
        cout << patientCount << " patients remaining: ";

        LinkedQueue<Patient*> tempPatients = Patients;
        Patient* tempPatient;

        int printCount = min(10, patientCount);
        for (int i = 0; i < printCount; i++) {
            if (tempPatients.dequeue(tempPatient)) {
                cout << "P" << tempPatient->getPID() << "_" << tempPatient->getVT();
                if (i < printCount - 1) cout << ", ";
            }
        }

        cout << "\n================ Waiting Lists ================" << endl;
        cout << E_Waiting_Patients.GetCount() << " E-therapy patients: ";
        E_Waiting_Patients.PrintQueue();
        cout << endl;

        cout << U_Waiting_Patients.GetCount() << " U-therapy patients: ";
        U_Waiting_Patients.PrintQueue();
        cout << endl;

        cout << X_Waiting_Patients.GetCount() << " X-therapy patients: ";
        X_Waiting_Patients.PrintQueue();
        cout << endl;

        cout << "================= Early List =================" << endl;
        cout << Early_Patients.GetCount() << " patients: ";
        Early_Patients.PrintPriQueue();
        cout << endl;

        cout << "================= Late List =================" << endl;
        cout << Late_Patients.GetCount() << " patients: ";
        Late_Patients.PrintPriQueue();
        cout << endl;

        cout << "================= E_Interrupted_Patients List =================" << endl;
        cout << E_Interrupted_Patients.GetCount() << " patients: ";
        E_Interrupted_Patients.PrintPriQueue();
        cout << endl;

        cout << "================= U_Interrupted_Patients List =================" << endl;
        cout << U_Interrupted_Patients.GetCount() << " patients: ";
        U_Interrupted_Patients.PrintPriQueue();
        cout << endl;

        cout << "================ Avail E-devices ================" << endl;
        int eCount = E_Devices.GetCount();
        cout << eCount << " Electro device: ";
        E_Devices.PrintQueue();
        cout << endl;

        cout << "================ Avail U-devices ================" << endl;
        int uCount = U_Deivces.GetCount();
        cout << uCount << " Ultra device: ";
        U_Deivces.PrintQueue();
        cout << endl;

        cout << "================ Avail X-rooms ================" << endl;
        int xCount = X_Deivces.GetCount();
        cout << xCount << " rooms: ";
        X_Deivces.PrintQueue();
        cout << endl;

        cout << "================ E_Maintenance_Devices ================" << endl;
        int eMinCount = E_Maintenance_Devices.GetCount();
        cout << eMinCount << " Electro device In Maintenance: ";
        E_Maintenance_Devices.PrintPriQueue();
        cout << endl;

        cout << "================ U_Maintenance_Devices ================" << endl;
        int uMinCount = U_Maintenance_Devices.GetCount();
        cout << uMinCount << " Ultra device In Maintenance: ";
        U_Maintenance_Devices.PrintPriQueue();
        cout << endl;

        cout << "============== In-treatment List ================" << endl;
        int inTreatmentCount = In_Treatment.GetCount();
        cout << inTreatmentCount << " ==> ";
        In_Treatment.PrintPriQueue();

        cout << "\n------------------------------------------------" << endl;

        int finishedCount = Finished_Patients.GetCount();
        cout << finishedCount << " finished patients: ";
        Finished_Patients.PrintStack();

        cout << "Press any key to display next timestep" << endl;
        cin.ignore();
    }

    void outputfile(ArrayStack<Patient*>& Patients, 
        LinkedQueue<Resources*>& E_Devices, 
        LinkedQueue<Resources*>& U_Deivces, 
        priQueue<Resources*>& E_Maintenance_Devices, 
        priQueue<Resources*>& U_Maintenance_Devices, 
        int timestep)
    {
        string filename;

        cout << "Enter file name (without extension): ";
        cin >> filename;
        filename = "output/" + filename + ".txt";

        ofstream outFile(filename);
        if (!outFile) {
            cout << "Error creating file!" << endl;
            return;
        }

        outFile << "PID  PType PT   VT   FT   WT   TT   Cancel Resc" << endl;
        float totalnumber, Nnumber, Rnumber, Wall, WR, WN, Tall, TN, TR, accCP, accRP, FDF, E_FDF, U_FDF;
        float Pearly, Plate, latePen;

        totalnumber =  Nnumber = Rnumber = Wall = WR = WN = Tall = TN = TR = accCP = accRP = Pearly = Plate = latePen = E_FDF = U_FDF = FDF = 0;

        while (!Patients.isEmpty())
        {
            Patient* temp;
            Patients.pop(temp);

            if (temp->getPID() < 10)
            {
                outFile <<"P" << temp->getPID() << "   ";
            }
            else if (temp->getPID() < 100)
            {
                outFile << "P" << temp->getPID() << "  ";
            }
            else
            {
                outFile << "P" << temp->getPID() << " ";
            }
            if(temp->istypeN())
                outFile << "N" << "     ";
            if (temp->istypeR())
                outFile << "R" << "     ";

            if (temp->getPT() < 10)
            {
                outFile << temp->getPT() << "    ";
            }
            else if(temp->getPT() < 100)
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
            
        }

        int E_Count = E_Devices.GetCount();
        int U_Count = U_Deivces.GetCount();
        while (!E_Devices.isEmpty())
        {
            Resources* R;
            if (E_Devices.dequeue(R))
                E_FDF += R->getFT();
        }

        while (!U_Deivces.isEmpty())
        {
            Resources* R;
            if (U_Deivces.dequeue(R))
                U_FDF += R->getFT();
        }

        Wall /= totalnumber;
        Tall /= totalnumber;

        WN /= Nnumber;
        TN /= Nnumber;

        WR /= Rnumber;
        TR /= Rnumber;

        accCP = accCP / totalnumber * 100;
        accRP = accRP / totalnumber * 100;

        FDF = ((E_FDF + U_FDF + (E_Maintenance_Devices.GetCount() + U_Maintenance_Devices.GetCount())) / (E_Count + U_Count + E_Maintenance_Devices.GetCount() + U_Maintenance_Devices.GetCount())) * 100;

        outFile << "\nTotal number of timesteps = " << timestep << endl;
        outFile << "Total number of All, N, R = " << totalnumber << ", " << Nnumber << ", " << Rnumber << endl;
        outFile << "Average total waiting time of All, N, R = " << Wall << ", " << WN << ", " << WR << endl;
        outFile << "Average total treatment time  of All, N, R = " << Tall << ", " << TN << ", " << TR << endl;
        outFile << "Percentage of patients that accepted cancellation (%) = " << accCP << " %\n";
        outFile << "Percentage of patients that accepted rescheduling (%) = " << accRP << " %\n";

        //--------------------------------------------------------Bouns-------------------------------------------------------//
        outFile << "Percentage of free devices that failed (%) = " << FDF << " %\n";
        //outFile << "Percentage of patients that accepted rescheduling (%) = " << accRP << " %\n";
        //--------------------------------------------------------Bouns-------------------------------------------------------//


        outFile << "there is line here for early % aka Pearly\n";
        outFile << "there is line here for late % aka Plate\n";
        outFile << "there is line here for avg late pen aka latePen\n";

        outFile.close();

        cout << "\nFile created at: " << filename << endl;

        return;
    };
};
