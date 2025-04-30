#pragma once
#include "Resources.h"
class Scheduler;
class Patient;
//Treatment
//Base class for all treatment types required by the patient. Each treatment has a duration, an assignment time (the time it is assigned to a resource), and a pointer to the assigned resource. It should have two virtual functions:
//
//CanAssign: checks if the required resource is available or not.
//
//MoveToWait: moves the patient to the appropriate waiting list. It should call the appropriate AddToWait function from the scheduler class.
//
//Derive the treatment types from this class and override its virtual functions.
class Treatment {
protected:
	int duration;
	int assignmenttime;
	Resources* assignedResource;
	Type t;
public:
	Treatment(int duration);
	int getDuration() const;
	void setDuration(int d);
	//--------------------------------Bouns----------------------------------------//
	int getAssignmentTime() const;
	void setAssignmentTime(int d);
	//--------------------------------Bouns----------------------------------------//
	Resources*& getAssignedResource();
	void setAssignedResource(Resources*& r);
	void setTypet(Type t);
	Type getTypet();
	virtual bool CanAssign(Scheduler* s) = 0;
	virtual void MoveToWait(Scheduler* s, Patient* p, int t) = 0;
};

class XTherapyTreatment : public Treatment {
public:
	XTherapyTreatment(int duration);
	virtual bool CanAssign(Scheduler* s);
	virtual void MoveToWait(Scheduler* s, Patient* p, int t);
};
class ETherapyTreatment : public Treatment {
public:
	ETherapyTreatment(int duration);
	virtual bool CanAssign(Scheduler* s);
	virtual void MoveToWait(Scheduler* s, Patient* p, int t);
};
class UTherapyTreatment : public Treatment
{
public:
	UTherapyTreatment(int duration);
	virtual bool CanAssign(Scheduler* s);
	virtual void MoveToWait(Scheduler* s, Patient* p, int t);
};