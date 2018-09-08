#include <Windows.h>



int MyBadFunction(int param)
{
	if (param % 3 == 0)
	{
		int * p = (int *)param;
		*p = 0;
	}
	return 0;
}