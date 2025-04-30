#pragma once
#include <iostream> // Include the iostream header for cout
using namespace std;
enum Type { X, E, U }; //X for Gym, E for ETherapy, U for UTherapy;

class Resources
{
protected:
	int cap;
	int attachedPatientsCount = 0; // Number of patients attached to the gym
	int ID;
	Type type;
	//--------------------------------------------------------Bouns-------------------------------------------------------//
	int Maintenance_Time;
public:

	int getMaintenance_Time()
	{
		return Maintenance_Time;
	}

	//--------------------------------------------------------Bouns-------------------------------------------------------//
	virtual Type getType() const
	{
		return type;
	}
	bool isFull() const {
		return attachedPatientsCount == cap;
	}
	bool incAttachedPatientsCount() {
		if (attachedPatientsCount < cap) {
			attachedPatientsCount++;
			return true;
		}
		else {
			return false;
		}
	}
	bool decAttachedPatientsCount() {
		if (attachedPatientsCount > 0) {
			attachedPatientsCount--;
			return true;
		}
		else {
			return false;
		}
	}
	int getCap() const
	{
		return cap;
	}
	virtual void printForResources() const
	{
		if (type == X)
			cout << "R" << this->ID << "[" << attachedPatientsCount << "," << cap << "]";
		else if (type == E)
			cout << "E" << this->ID;
		else if (type == U)
			cout << "U" << this->ID;
	}

	virtual void printForPatient() const
	{
		if (type == X)
			cout << "R" << this->ID;
		else if (type == E)
			cout << "E" << this->ID;
		else if (type == U)
			cout << "U" << this->ID;
	}
};

class Gym : public Resources
{
private:
	static int nextID;
public:
	Gym() {
		type = X;
		cap = 0;
		ID = nextID++;
	}// Default constructor
	Gym(int c) {
		setCap(c);
		type = X;
	}
	void setCap(int n)
	{
		if (n > 0)
			cap = n;
		else
			cap = 0;
	}
	int getnumber()
	{
		return attachedPatientsCount;
	}
};


class ETherapy : public Resources
{
private:
	static int nextID;
public:
	ETherapy(int MT) {
		cap = 1;
		type = E;
		ID = nextID++;
		Maintenance_Time = MT;
	}
};

class UTherapy : public Resources
{
private:
	static int nextID;
public:
	UTherapy(int MT) {
		cap = 1;
		type = U;
		ID = nextID++;
		Maintenance_Time = MT;
	}
};
ostream& operator<<(ostream& os, const Resources* p);