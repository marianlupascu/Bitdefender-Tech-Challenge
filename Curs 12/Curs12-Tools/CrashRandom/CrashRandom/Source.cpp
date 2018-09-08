#include <Windows.h>


int main()
{
	srand(GetTickCount());
	if (rand() % 3 == 0)
	{
		int * p = 0;
		*p = 5;
	}
}