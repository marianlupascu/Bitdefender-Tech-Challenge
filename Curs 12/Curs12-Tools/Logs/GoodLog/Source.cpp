#include <stdio.h>
#include <string.h>

char * text_to_write = "Logul meu - linia ";

int main()
{
	FILE * f = NULL;
	fopen_s(&f, "good.log", "a+");
	for (int i = 0; i < 1000000; i++)
	{
		fprintf(f, "%s %d",text_to_write, i);
		fprintf(f, "%c", '\n');
		
	}
	fclose(f);
	return 0;

}