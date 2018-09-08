#include <Windows.h>
#include <stdio.h>
#include <string.h>

char * text_to_write = "Logul meu - linia ";

int main()
{
	//::MessageBox(NULL, L"lala", L"lala", 0);
	for (int i = 0; i < 1000000; i++)
	{
		FILE * f = NULL;
		fopen_s(&f, "bad.log", "a+");
		if (!f)
		{
			printf("could not open file\n");
			i--;
			continue;
		}
		for (int j = 0; j < strlen(text_to_write); j++)
			fprintf(f, "%c", text_to_write[j]);
		fprintf(f, "%d", i);
		fprintf(f, "%c", '\n');
		fclose(f);
	}

	return 0;

}