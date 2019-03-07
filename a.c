/*
Кочнов Андрей
АПО-12


Задача A-6. Задача о приведении строк к нижнему регистру символов

Составить программу построчной обработки текста. Суть обработки - приведение каждого символа в каждой строке к нижнему регистру.

Программа считывает входные данные со стандартного ввода, и печатает результат в стандартный вывод.

Процедура приведения к нижнему регистру должна быть оформлена в виде отдельной функции, которой на вход подается массив строк (который необходимо обработать), количество переданных строк, а также указатель на переменную, в которой необходимо разместить результат - массив уже обработанных строк.
В качестве возвращаемого значения функция должна возвращать количество строк, содержащихся в результирующем массиве.

Программа должна уметь обрабатывать ошибки - такие как неверные входные данные(отсутствие входных строк) или ошибки выделения памяти и т.п.
В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и завершать выполнение программы.
*/

#include <stdio.h>
#include <stdlib.h>
char* inputString(int* isEOF);
char** inputStrings(int* sCount);
unsigned int toLower(const char** origin, const int sCount, char** dest);
void deleteStringArray(char** sArray, int strCount);

int main()
{
	int strCount = 0;
	char** strArray = inputStrings(&strCount);
	if (!strArray) return 0;

	toLower((const char**)strArray, strCount, strArray);
	if (strArray){
		for (int i = 0; i < strCount; i++) {
			printf("%s\n", strArray[i]);
		}
	deleteStringArray(strArray, strCount);
	}
	return 0;
}

unsigned int toLower(const char** const origin, const int sCount, char** const dest) {
	if (!origin || !*(origin) || !dest || !(*dest)) {
		return 0;
	}
	unsigned int i = 0;      //индекс строки в массиве

	for (; i < sCount && origin[i] && dest[i]; i++) {    //по массиву строк
		unsigned int j = 0;  //индекс символа в строке

		for (; origin[i][j]; j++) {     //по строке
			if (origin[i][j] >= 'A' && origin[i][j] <= 'Z')
				dest[i][j] = origin[i][j] - 'A' + 'a';
		}
		dest[i][j] = '\0';
	}
	return i + 1;
}

char* inputString(int* isEOF) {
	struct Buffer {
		char* str;
		size_t dataSize;
		size_t spaceSize;
	} buf = { NULL, 0, 0 };
	const unsigned int strSizeFactor = 2;
	char c = '0';
	*isEOF = 0;
	while (c = getc(stdin), c != '\n' && c != EOF) {
		if ((buf.dataSize + 2) > buf.spaceSize) {		//current + zero-symbol
			size_t newBufferSize = buf.spaceSize ? buf.spaceSize*strSizeFactor : 2;
			char* temp = (char*)realloc(buf.str, newBufferSize);
			if (!temp) {
				fprintf(stderr, "[error]");
				free(buf.str);
				return NULL;
			}
			buf.str = temp;
			buf.spaceSize = newBufferSize;
		}
		buf.str[buf.dataSize++] = c;
	}
	if (buf.str) {
		buf.str[buf.dataSize++] = '\0';
		if (buf.dataSize < buf.spaceSize) {
			char* temp = (char*)realloc(buf.str, buf.dataSize);
			if (!temp) {
				fprintf(stderr, "[error]");
			}
			else {
				buf.str = temp;
			}
		}
	}
	if (c == EOF) {
		*isEOF = !(*isEOF);
	}
	return buf.str;
}

char** inputStrings(int* sCount) {
	struct StringBuffer {
		char** strArray;
		unsigned int dataSize;
		unsigned int spaceSize;
	} sBuf = { NULL, 0, 0 };
	const unsigned int strSizeFactor = 2;
	int isEOF = 0;
	char* str = NULL;
	while (!isEOF && (str = inputString(&isEOF), str)) {
		if ((sBuf.dataSize + 1) > sBuf.spaceSize) {
			size_t newBufferSize = sBuf.dataSize ? sBuf.dataSize*strSizeFactor : 1;

			char** temp = (char**)realloc(sBuf.strArray, newBufferSize * sizeof(char*));
			if (!temp) {
				fprintf(stderr, "[error]");
				str = NULL;
				break;
			}
			sBuf.spaceSize = newBufferSize;
			sBuf.strArray = temp;
		}
		sBuf.strArray[sBuf.dataSize++] = str;
		str = NULL;
		//if (isEOF) break;
	}
	if (isEOF == 0 && !str) {	//произошла ошибка
		deleteStringArray(sBuf.strArray, sBuf.dataSize);
		return NULL;
		*sCount = 0;
	}
	if (sBuf.dataSize < sBuf.spaceSize) {
		char** temp = (char**)realloc(sBuf.strArray, sBuf.dataSize*sizeof(char*));
		if (!temp) {
			fprintf(stderr, "[error]");
		}
		else {
			sBuf.strArray = temp;
		}
	}
	*sCount = sBuf.dataSize;
	return sBuf.strArray;

}

void deleteStringArray(char** sArray, int strCount) {
	if (sArray) {
		for (int i = 0; i < strCount; i++) {
			free(sArray[i]);
		}
		free(sArray);

	}
}
