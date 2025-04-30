#include "patient.h"

// Define static variable for Patient
int Patient::PIDs = 0;

std::ostream& operator<<(ostream& os, const Patient* p)
{
	p->Print();
	return os;
}