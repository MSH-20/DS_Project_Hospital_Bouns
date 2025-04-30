#include "Resources.h"

// Define static variables for Gym, ETherapy, and UTherapy
int Gym::nextID = 1;
int ETherapy::nextID = 1;
int UTherapy::nextID = 1;

std::ostream& operator<<(ostream& os, const Resources* p)
{
	p->printForResources();
	return os;
}