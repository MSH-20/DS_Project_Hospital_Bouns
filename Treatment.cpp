#include "Treatment.h"
#include "Scheduler.h"
//Treatment
//Base class for all treatment types required by the patient. Each treatment has a duration, an assignment time (the time it is assigned to a resource), and a pointer to the assigned resource. It should have two virtual functions:
//
//CanAssign: checks if the required resource is available or not.
//
//MoveToWait: moves the patient to the appropriate waiting list. It should call the appropriate AddToWait function from the scheduler class.
//
//Derive the treatment types from this class and override its virtual functions.
Treatment::Treatment(int duration){
	this->duration = duration;
	assignmenttime = 0;
	assignedResource = nullptr;
}

//--------------------------------Bouns----------------------------------------//

int Treatment::getAssignmentTime() const
{
	return assignmenttime;
}
void Treatment::setAssignmentTime(int t)
{
	assignmenttime = t;
}
//--------------------------------Bouns----------------------------------------//
int Treatment::getDuration() const {
	return duration;
}
Resources*& Treatment::getAssignedResource() {
	return assignedResource;
}
void Treatment::setAssignedResource(Resources*& r) {
	assignedResource = r;
}
void Treatment::setTypet(Type t) {
	this->t = t;
}
Type Treatment::getTypet() {
	return t;
}
void Treatment::setDuration(int d) {
	duration = d;
}


XTherapyTreatment::XTherapyTreatment(int duration) : Treatment(duration){}
bool XTherapyTreatment::CanAssign(Scheduler* s)
{
	return s->getCount_X() > 0;
}
void XTherapyTreatment::MoveToWait(Scheduler* s, Patient* p,int t) {
	s->AddToWait_X(t,p);
}






ETherapyTreatment::ETherapyTreatment(int duration) : Treatment(duration)
{}
bool ETherapyTreatment ::CanAssign(Scheduler* s)
{
	return s->getCount_E() > 0;
}
void ETherapyTreatment :: MoveToWait(Scheduler * s,Patient* p,int t) {
	s->AddToWait_E(t,p);
}






UTherapyTreatment::UTherapyTreatment(int duration) :Treatment(duration)
{}
bool UTherapyTreatment ::CanAssign(Scheduler* s)
{
	return s->getCount_U() > 0;
}
void UTherapyTreatment::  MoveToWait(Scheduler* s, Patient* p,int t) {
	s->AddToWait_U(t,p);
}