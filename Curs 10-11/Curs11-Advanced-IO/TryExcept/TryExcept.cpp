#include <Windows.h>
#include <stdio.h>

int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
	printf("code=%x\n", code);
	return EXCEPTION_EXECUTE_HANDLER;
}


int main()
{
	int * p = NULL;

	__try
	{
		*p = 0x10;
	}
	__except (ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		printf("crash intercepted\n");
	}

	printf("continue execution.\n");
	return 0;
}