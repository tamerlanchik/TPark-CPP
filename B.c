/*
Кочнов Андрей, АПО-12

Задача B-5. Парсер, логические выражения.

Требуется написать программу, которая способна вычислять логическе выражения.
Допустимые выражения чувствительны к регистру и могут содержать:
1) знаки операций 'and' - и, 'or' - или, 'not' - отрицание, 'xor' - сложение по модулю 2
2) Скобки '(', ')'
3) Логические значения 'True', 'False'
4) Пустое выражение имеет значение "False"

Также требуется реализовать возможность задания переменных, которые могут состоять 
только из маленьких букв латинского алфавита (к примеру, 'x=True').
Объявление переменной имеет формат:
<имя>=<True|False>; // допустимы пробелы

Допускается несколько объявлений одной и той же переменной, учитывается последнее.
Все переменные, используемые в выражении, должны быть заданы заранее. Также запрещены
имена, совпадающие с ключевыми словами (and, or и т.д.).

Необходимо учитывать приоритеты операций. Пробелы ничего не значат.

Если выражение или объявление не удовлетворяют описанным правилам, требуется вывести 
в стандартный поток вывода "[error]" (без кавычек).

ВАЖНО! Программа в любом случае должна возвращать 0. Не пишите return -1, exit(1) и т.п.
Даже если обнаружилась какая-то ошибка, все равно необходимо вернуть 0!
(и напечатать [error] в stdout).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------Dynamic String implementation-------
typedef struct DynamicString_template {
	char* data;
	unsigned int spaceSize;
	unsigned int dataSize;
} DynString;
DynString* initDynamicString(void);
int append(DynString* const buf, const char data);
char pop(DynString* const buf);
int truncDynString(DynString* const dyn_array);
int freeDynamicString(DynString* const stack);

//------Dynamic Dictionary implementation----
typedef struct DynamicDict_template {
	char** str;
	int* values;
	unsigned int spaceSize;
	unsigned int dataSize;
} DynDict;
DynDict* initDynamicDict(void);
int appendDict(DynDict* const buf, char* const str, const int val);
int freeDynamicDict(DynDict* const dict);

//------Input--------------------------------
char* getNextInputString(FILE* const flow, int* isEOF);

//-------Problem solving functions------
int eatChars(int index, const int maxIndex, const char* const s, const char food);
int parseString(const char* const s, DynDict** const varList);
int parseManifestation(const char* const s, const int indexEq, char** variableName, int* variableValue);
int parseExpression(const char* const s, const DynDict* const varList, DynString** backPolandNotation);
int calculateExpresson(DynString* const s);

//-----Static string functions------------
int isLetter(const char c);
int isNumber(const char c);
int findInArray(const char* const what, const char** where, const int whereLen);

const char* boolConstants[] = { "False", "True" };
const char errorMessage[] = "[error]";
enum stateCodes { ERROR = -1, OK };
const char input_file_name[] = "input.txt";

#define PROD	//переключатель режима ввода (по условию задачи не требуется)

int main(void)
{
	FILE* f = stdin;
#ifndef PROD
	f = fopen(input_file_name, "r");
	if (!f) {
		printf("%s", errorMessage);
		return 0;
	}
#endif
	int isEOF = 0;
	char* str = NULL;
	DynDict* varList = initDynamicDict();
	int isCrashFlag = OK;

	while (!isEOF && (str = getNextInputString(f,&isEOF), str) ) {
		isCrashFlag = parseString(str, &varList);		//здесь же выводится ответ
		free(str);
		str = NULL;
		if (isCrashFlag == ERROR) { break; }
	}
	if (isCrashFlag == ERROR) {
		printf("%s", errorMessage);
	}
	freeDynamicDict(varList);
	if (f) { fclose(f); }
	return 0;
}

char* getNextInputString(FILE* const flow, int* isEOF) {
	DynString* buf = initDynamicString();
	if (!buf) {
		freeDynamicString(buf);
		return NULL;
	}
	if (!flow) {
		freeDynamicString(buf);
		return NULL;
	}
	char c = '0';
	while (c = (char)fgetc(flow), c != '\n' && c != EOF) {
		if (ERROR == append(buf, c))
		{
			freeDynamicString(buf);
			return NULL;
		}
	}
	if (ERROR == append(buf, '\0')) {
		freeDynamicString(buf);
		return NULL;
	}
	if (ERROR == truncDynString(buf)) {
		freeDynamicString(buf);
		return NULL;
	}
	if (c == EOF) {
		*isEOF = !(*isEOF);
	}
	char* tempPointer = buf->data;
	free(buf);
	return tempPointer;
}

//-------Problem solving functions------
int eatChars(int index, const int maxIndex, const char* const s, const char food) {
	if (!s) {
		return ERROR;
	}
	while (index < maxIndex && s[index] == food) {
		index++;
	}
	return index;
}

int parseString(const char* const s, DynDict** const varList) {
	char* indexEqualsPtr = strchr(s, '=');
	if (indexEqualsPtr == NULL) {
		//строка с выражением, либо ошибочная
		DynString* backPolandNotation = NULL;
		if (parseExpression(s, *varList, &backPolandNotation) == ERROR) {
			freeDynamicString(backPolandNotation);
			return ERROR;
		}
		int answer = calculateExpresson(backPolandNotation);
		if (answer == ERROR || answer < 0 || answer > 1) {
			freeDynamicString(backPolandNotation);
			return ERROR;
		}

		//-------Answer-----------------------
		printf("%s\n", boolConstants[answer]);
		//------------------------------------

		if (backPolandNotation) {
			if (freeDynamicString(backPolandNotation) == ERROR) {
				return ERROR;
			}
		}
	}
	else {
		//если строка верна, в ней - определение переменной
		char* varName = NULL;
		int varValue = ERROR;
		int index = (int)(indexEqualsPtr - s);	//номер в строке знака '='
		indexEqualsPtr = NULL;
		if (parseManifestation(s, index, &varName, &varValue) == ERROR) {
			if (varName) { free(varName); }
			return ERROR;
		}
		int pos = findInArray(varName, (const char**)(*varList)->str, (*varList)->dataSize);
		if (pos < 0) {		//новая переменная
			appendDict(*varList, varName, varValue);
		}
		else {				//переопределение существующей переменной
			(*varList)->values[pos] = varValue;
			if (varName) { free(varName); }
		}
		varName = NULL;
	}
	return OK;
}

int parseManifestation(const char* const s, const int indexEq, char** variableName, int* variableValue) {
	const int reservedWordsCount = 6;
	const char* reservedWords[] = { "and", "or", "not", "xor", "False", "True" };

	DynString* varName = initDynamicString();
	unsigned int varValue = 0;
	int j = 0;		//текущий индекс в строке
	int sLen = strlen(s);
	if (sLen < 0) {
		freeDynamicString(varName);
		return ERROR;
	}

	j = eatChars(j, indexEq, s, ' ');		//движемся к первой букве
	//собираем слово-название до знака равенства
	while (j < indexEq && isLetter(s[j])) {
		if (append(varName, s[j]) == OK)
			j++;
		else
			return ERROR;
	}
	if (varName->dataSize == 0) {
		freeDynamicString(varName);
		return ERROR;
	}
	append(varName, '\0');;
	if (findInArray(varName->data, reservedWords, reservedWordsCount) >= 0) {
		freeDynamicString(varName);
		return ERROR;
	}
	j = eatChars(j, indexEq, s, ' ');	//движемся к знаку равно
	if (j == indexEq) { j++; }			//пропускаем знак равно
	else {
		freeDynamicString(varName);
		return ERROR;
	}
	j = eatChars(j, sLen, s, ' ');		//движемся к определителю переменной
	DynString* valueName = initDynamicString();
	while (j < sLen && isLetter(s[j])) {	//собираем имя определителя
		append(valueName, s[j]);
		j++;
	}
	append(valueName, '\0');
	int ind = findInArray(valueName->data, boolConstants, 2);
	freeDynamicString(valueName);
	if (ind == ERROR) {
		freeDynamicString(varName);
		return ERROR;
	}
	else {
		varValue = ind;		//False = 0, True = 1
	}
	j = eatChars(j, sLen, s, ' ');		//Движемся до ';'
	if (j < sLen - 2) {		//если указатель стоит раньше последнего значащего символа
		freeDynamicString(varName);
		return ERROR;
	}
	*variableName = varName->data;
	free(varName);
	*variableValue = varValue;
	return OK;
}

int parseExpression(const char* const s, const DynDict* const varList, DynString** backPolandNotation) {
	const char* operatorNamesList[] = { "and", "or", "not", "xor", "(", ")" };
	const int operatorsCount = 4;
	const char operatorShortcuts[] = { '*', '+', '~', '^' };
	const int operRates[] = { -1, 2, 1, 2, 3, 0, 5 };
						//	   ?  *  +  ^  ~  (  )
	const char operList[] = "?*+^~()";

	if (!s || !varList) { return ERROR; }

	DynString* stack = initDynamicString();
	append(stack, '?');		// '?' имеет низший приоритет, проще обрабатывать конец стека
	DynString* output = initDynamicString();
	DynString* val = initDynamicString();
	int strLen = strlen(s);
	int j = 0;
	int isCrashFlag = OK;
	for (; j<=strLen; j++) {
		if (isLetter(s[j])) {
			append(val, s[j]);
		}
		else {
			if(val->dataSize > 0){
				append(val, '\0');
				//Что за слово получено?

				//1) Переменная?
				int index = findInArray(val->data, (const char**)varList->str, varList->dataSize);
				if (index >= 0) {
					append(output, (char)varList->values[index] + '0');
				} 
				//2) Константа?
				else if (strcmp("True", val->data) == 0) {
					append(output, '1');
				}
				else if (strcmp("False", val->data) == 0) {
					append(output, '0');
				}
				//3) Оператор (буквенный)?
				else {
					index = findInArray(val->data, operatorNamesList, operatorsCount);
					if (index < 0) {
						isCrashFlag = ERROR;
						break;
					}
					//обрабатываем оператор
					char oper = operatorShortcuts[index];
					char temp = pop(stack);
					if (!temp) {
						isCrashFlag = ERROR;
						break;
					}
					int tempIndex = (int)(strchr(operList, temp) - operList);
					index = (int)(strchr(operList, oper) - operList);

					while (operRates[index] <= operRates[tempIndex]) {
						append(output, temp);
						temp = pop(stack);
						if (!temp) {
							isCrashFlag = ERROR;
							break;
						}
						tempIndex = (int)(strchr(operList, temp) - operList);
					}
					append(stack, temp);
					append(stack, oper);
				}
				val->dataSize = 0;
			}

			//что за символ попался?
			if (s[j] == '(') {
				append(stack, s[j]);
			}
			else if (s[j] == ')') {
				char temp;
				while (temp = pop(stack), temp!= '\0' && temp != '(') {
					append(output, temp);
				}
				if (!temp) {
					isCrashFlag = ERROR;
					break;
				}
			}
			else if (s[j] != ' ' && s[j] != '\0') {
				isCrashFlag = ERROR;
				break;
			}
			//если пробел - идём дальше
		}
	}
	freeDynamicString(val);
	val = NULL;
	if (isCrashFlag == OK) {
		while (stack->dataSize > 1) {
			//в стеке должны остаться только операторы
			char temp = pop(stack);
			if (!temp) {
				isCrashFlag = ERROR;
				break;
			}
			//нижеперечисленных символов быть не должно
			else if (temp == '(' || temp == ')' || isNumber(temp) == OK) {
				isCrashFlag = ERROR;
				break;
			}
			/*else if (temp >= '0' && temp <= '9') {
				isCrashFlag = ERROR;
				break;
			}*/
			append(output, temp);
		}
	}
	if (isCrashFlag == OK) {
		*backPolandNotation = output;
	}
	else {
		freeDynamicString(output);
	}
	if (freeDynamicString(stack) == ERROR) {
		isCrashFlag = ERROR;
	}
	return isCrashFlag;
}

int calculateExpresson(DynString* const s) {
	if (!s) { return ERROR; }

	DynString* stack = initDynamicString();
	int isCrashFlag = OK;
	for (unsigned int i = 0; i < s->dataSize; i++) {
		if (s->data[i] >= '0' && s->data[i] <= '9') {
			append(stack, s->data[i]);
		}
		else {
			if (stack->dataSize == 0) {
				isCrashFlag = ERROR;
				break;
			}
			char a = pop(stack) - '0';
			if (s->data[i] == '~') {
				append(stack, '0' + (!a));
			}
			else {
				char b = pop(stack) - '0';
				switch (s->data[i]) {
				case '+':
					a = a | b;
					break;
				case '*':
					a = a & b;
					break;
				case '^':
					a = a ^ b;
					break;
				default:
					isCrashFlag = ERROR;
					break;
				}
				append(stack, '0' + a);
			}
		}
	}
	int ans = ERROR;
	if (isCrashFlag == OK) {
		ans = pop(stack) - '0';
		if (stack->dataSize != 0 || (ans < 0 || ans > 1)) {
			ans = ERROR;
		}
	}
	freeDynamicString(stack);
	return ans;
}

//-----Dynamic Array Implementation-----------
int append(DynString* const buf, const char data) {
	if (!buf) { return ERROR; }

	const int strSizeFactor = 2;
	if ((buf->dataSize + 1) > buf->spaceSize) {		//current + zero-symbol
		size_t newBufferSize = buf->spaceSize ? buf->spaceSize*strSizeFactor : 1;
		char* temp = (char*)realloc(buf->data, newBufferSize * sizeof(char));
		if (!temp) {
			free(buf->data);
			return ERROR;
		}
		buf->data = temp;
		buf->spaceSize = newBufferSize;
	}
	buf->data[buf->dataSize++] = data;
	return OK;
}

int appendDict(DynDict* const buf, char* const str, const int val) {
	if (!buf) { return ERROR; }
	const int strSizeFactor = 2;
	if ((buf->dataSize + 1) > buf->spaceSize) {		//current + zero-symbol
		size_t newBufferSize = buf->spaceSize ? buf->spaceSize*strSizeFactor : 1;
		char** tempStr = (char**)realloc(buf->str, newBufferSize * sizeof(char*));
		int* tempInt = (int*)realloc(buf->values, newBufferSize * sizeof(int));
		if (!tempStr || !tempInt) {
			freeDynamicDict(buf);
			return ERROR;
		}
		buf->str = tempStr;
		buf->values = tempInt;
		buf->spaceSize = newBufferSize;
	}
	buf->str[buf->dataSize] = str;
	buf->values[buf->dataSize] = val;
	buf->dataSize++;
	return OK;
}

char pop(DynString* const buf) {
	if (!buf || buf->dataSize == 0) {
		return '\0';
	}
	return buf->data[--(buf->dataSize)];
}

int isEmpty(const DynString* const stack) {
	if (!stack) { return ERROR; }

	if (stack->dataSize == 0) {
		return ERROR;
	}
	else {
		return OK;
	}
}

int freeDynamicString(DynString* const stack) {
	if (!stack) { return ERROR; }

	free(stack->data);
	free(stack);
	return OK;
}

int freeDynamicDict(DynDict* const dict) {
	if (!dict) { return ERROR; }
	for (unsigned int i = 0; i < dict->dataSize; i++) {
		free(dict->str[i]);
	}
	free(dict->str);
	free(dict->values);
	free(dict);
	return OK;
}

int truncDynString(DynString* const dyn_array) {
	if (!dyn_array) { return ERROR; }
	if (dyn_array->dataSize < dyn_array->spaceSize) {
		char* temp = (char*)realloc(dyn_array->data, dyn_array->dataSize * sizeof(char));
		if (!temp && dyn_array->dataSize != 0) {
			return ERROR;
		}
		dyn_array->data = temp;
		dyn_array->spaceSize = dyn_array->dataSize;
		return OK;
	}
	return OK;
}

DynString* initDynamicString(void) {
	DynString* arr = (DynString*)malloc(sizeof(DynString));
	if (!arr) { return NULL; }
	arr->data = NULL;
	arr->dataSize = 0;
	arr->spaceSize = 0;
	return arr;
}

DynDict* initDynamicDict(void) {
	DynDict* arr = (DynDict*)malloc(sizeof(DynDict));
	if (!arr) { return NULL; }
	arr->str = NULL;
	arr->values = NULL;
	arr->dataSize = 0;
	arr->spaceSize = 0;
	return arr;
}

int isLetter(const char c) {
	if (c >= 'A' && c <= 'z') {
		return 1;
	}
	else {
		return 0;
	}
}

int isNumber(const char c) {
	if (c >= '0' && c <= '9') {
		return OK;
	}
	else {
		return ERROR;
	}
}

int findInArray(const char* const what, const char** where, const int whereLen) {
	if (whereLen <= 0) { return ERROR; }
	if (!what || !where || !(*where)) { return ERROR; }

	for (int j = 0; j < whereLen; j++) {
		if (strcmp(what, where[j]) == 0) {
			return j;
		}
	}
	return ERROR;
}
