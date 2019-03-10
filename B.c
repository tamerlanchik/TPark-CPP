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
typedef struct Dynamic_String_template {
	char* data;
	unsigned int spaceSize;
	unsigned int dataSize;
} DynString;
DynString* init_dynamic_string(void);
int append_string(DynString* const buf, const char data);
char pop(DynString* const buf);
int trunc_dynamic_string(DynString* const dynamic_array);
int free_dynamic_string(DynString* const stack);

//------Dynamic Dictionary implementation----
typedef struct Dynamic_Dict_template {
	char** str;
	int* values;
	unsigned int spaceSize;
	unsigned int dataSize;
} DynDict;
DynDict* init_dynamic_dict(void);
int append_dict(DynDict* const buf, char* const str, const int val);
int free_dynamic_dict(DynDict* const dict);

//------Input--------------------------------
char* get_next_input_string(FILE* const flow, int* is_eof);

//-------Problem solving functions------
int eat_chars(int index, const int border_index, const char* const s, const char food);

int parse_string(const char* const s, DynDict** const var_list);

int parse_variable_definition(const char* const s, const int index_equal, 
								char** variable_name, int* variableValue);

int parse_expression(const char* const s, const DynDict* const var_list, 
						DynString** back_poland_notation);

int calculate_expression(DynString* const s);

//-----Static string functions------------
int is_letter(const char c);

int is_number(const char c);

int find_string_in_array(const char* const what, const char** where, const int where_len);
//----------------------------------------

const char* BOOL_CONSTANTS[] = { "False", "True" };
const char ERROR_MESSAGE[] = "[error]";
enum status_codes { ERROR = -1, OK };
const char INPUT_FILE_NAME[] = "input.txt";

#define PROD	//переключатель режима ввода (по условию задачи не требуется)

int main(void)
{
	FILE* f = stdin;

	#ifndef PROD
	f = fopen(INPUT_FILE_NAME, "r");
	if (!f) {
		printf("%s", ERROR_MESSAGE);
		return 0;
	}
	#endif

	int is_eof = 0;
	char* str = NULL;
	DynDict* var_list = init_dynamic_dict();
	int was_crash = OK;

	while (!is_eof && (str = get_next_input_string(f,&is_eof), str) ) {
		was_crash = parse_string(str, &var_list);		//здесь же выводится ответ
		free(str);
		str = NULL;
		if (was_crash == ERROR) { break; }
	}
	if (was_crash == ERROR) {
		printf("%s", ERROR_MESSAGE);
	}
	free_dynamic_dict(var_list);
	if (f) { fclose(f); }

	return 0;
}

char* get_next_input_string(FILE* const flow, int* is_eof) {
	if (!flow) { return NULL; }

	DynString* buf = init_dynamic_string();		//буфер ввода
	if (!buf) { return NULL; }

	char c = '\0';
	while (c = (char)fgetc(flow), c != '\n' && c != EOF) {
		if (ERROR == append_string(buf, c))
		{
			free_dynamic_string(buf);
			return NULL;
		}
	}
	if (ERROR == append_string(buf, '\0')) {
		free_dynamic_string(buf);
		return NULL;
	}
	if (ERROR == trunc_dynamic_string(buf)) {
		free_dynamic_string(buf);
		return NULL;
	}
	if (EOF == c) {
		*is_eof = !(*is_eof);
	}
	char* tempPointer = buf->data;
	free(buf);
	return tempPointer;
}

//-------Problem solving functions------

//  Функция сдвигает маркер, пока не встретит заданный символ
int eat_chars(int index, const int border_index, const char* const s, const char food) {
	if (!s) { return ERROR; }

	while (index < border_index && s[index] == food) {
		index++;
	}
	return index;
}

//	Анализирует и обрабатывает входные строки
int parse_string(const char* const s, DynDict** const var_list) {
	char* index_equal_ptr = strchr(s, '=');
	if (index_equal_ptr == NULL) {
		//строка с выражением либо ошибочная
		DynString* back_poland_notation = NULL;
		if (ERROR == parse_expression(s, *var_list, &back_poland_notation)) {
			free_dynamic_string(back_poland_notation);
			return ERROR;
		}

		int answer = calculate_expression(back_poland_notation);
		if (ERROR == answer || answer < 0 || answer > 1) {
			free_dynamic_string(back_poland_notation);
			return ERROR;
		}

		//-------Answer-----------------------
		printf("%s\n", BOOL_CONSTANTS[answer]);
		//------------------------------------

		if (back_poland_notation) {
			if (ERROR == free_dynamic_string(back_poland_notation)) {
				return ERROR;
			}
		}
	}
	else {
		//если строка верна, в ней - определение переменной
		char* var_name = NULL;
		int varValue = ERROR;
		int index = (int)(index_equal_ptr - s);	//номер в строке знака '='

		if (ERROR == parse_variable_definition(s, index, &var_name, &varValue)) {
			if (var_name) { free(var_name); }
			return ERROR;
		}
		int pos = find_string_in_array(var_name, (const char**)(*var_list)->str, (*var_list)->dataSize);
		if (pos < 0) {		//новая переменная
			append_dict(*var_list, var_name, varValue);
		}
		else {				//переопределение существующей переменной
			(*var_list)->values[pos] = varValue;
			if (var_name) { free(var_name); }
		}
		var_name = NULL;
	}
	return OK;
}

//	Обрабатывает определения переменных
int parse_variable_definition(const char* const s, const int index_equal, 
							char** variable_name, int* variableValue) {
	const int reservedWordsCount = 6;
	const char* reservedWords[] = { "and", "or", "not", "xor", "False", "True" };

	DynString* var_name = init_dynamic_string();
	if (!var_name) { return ERROR; }
	unsigned int varValue = 0;
	int j = 0;		//текущий индекс в строке
	int sLen = strlen(s);
	if (sLen < 0) {
		free_dynamic_string(var_name);
		return ERROR;
	}

	j = eat_chars(j, index_equal, s, ' ');		//движемся к первой букве
	//собираем слово-название до знака равенства
	while (j < index_equal && is_letter(s[j])) {
		if (append_string(var_name, s[j]) == OK)
			j++;
		else {
			free_dynamic_string(var_name);
			return ERROR;
		}			
	}
	if (var_name->dataSize == 0) {
		free_dynamic_string(var_name);
		return ERROR;
	}
	if (ERROR == append_string(var_name, '\0')) {
		free_dynamic_string(var_name);
		return ERROR;
	}
	if (find_string_in_array(var_name->data, reservedWords, reservedWordsCount) >= 0) {
		free_dynamic_string(var_name);
		return ERROR;
	}
	j = eat_chars(j, index_equal, s, ' ');	//движемся к знаку равно
	if (j == index_equal) { j++; }			//пропускаем знак равно
	else {
		free_dynamic_string(var_name);		//если м/у названием перем. и '=' есть символы
		return ERROR;
	}
	j = eat_chars(j, sLen, s, ' ');		//движемся к определителю переменной

	DynString* value_name = init_dynamic_string();
	if (!value_name) {
		free_dynamic_string(var_name);
		return ERROR;
	}
	while (j < sLen && is_letter(s[j])) {	//собираем имя определителя
		if (ERROR == append_string(value_name, s[j]))
		{
			free_dynamic_string(var_name);
			free_dynamic_string(value_name);
			return ERROR;
		}
		j++;
	}
	if (ERROR == append_string(value_name, '\0'))
	{
		free_dynamic_string(var_name);
		free_dynamic_string(value_name);
		return ERROR;
	}
	int ind = find_string_in_array(value_name->data, BOOL_CONSTANTS, 2);
	if (ind == ERROR) {
		free_dynamic_string(var_name);
		return ERROR;
	}
	varValue = ind;		//False = 0, True = 1

	if (ERROR == free_dynamic_string(value_name)) {
		free_dynamic_string(var_name);
		return ERROR;
	}

	j = eat_chars(j, sLen, s, ' ');		//Движемся до ';'
	if (j < sLen - 2) {		//если указатель стоит раньше последнего значащего символа
		free_dynamic_string(var_name);
		return ERROR;
	}
	*variable_name = var_name->data;
	free(var_name);
	*variableValue = varValue;
	return OK;
}

//	Обрабатывает логические выражения
int parse_expression(const char* const s, const DynDict* const var_list, 
						DynString** back_poland_notation) {
	const char* operatorNamesList[] = { "and", "or", "not", "xor", "(", ")" };
	const int operatorsCount = 4;
	const char operatorShortcuts[] = { '*', '+', '~', '^' };
	const int operRates[] = { -1, 2, 1, 2, 3, 0, 5 };
						//	   ?  *  +  ^  ~  (  )
	const char operList[] = "?*+^~()";

	if (!s || !var_list || !back_poland_notation) {
		return ERROR; 
	}

	DynString* stack = init_dynamic_string();
	if (!stack) { return ERROR; }

	if (ERROR == append_string(stack, '?')) {	// '?' имеет низший приоритет, проще обрабатывать конец стека
		free_dynamic_string(stack);
		return ERROR;
	}

	DynString* output = init_dynamic_string();
	if (!output) { 
		free_dynamic_string(stack);
		return ERROR; 
	}
	DynString* val = init_dynamic_string();
	if (!val) { 
		free_dynamic_string(stack);
		free_dynamic_string(output);
		return ERROR; 
	}

	int strLen = strlen(s);
	int j = 0;
	int was_crash = OK;
	for (; j<=strLen; j++) {
		if (is_letter(s[j])) {
			if (ERROR == append_string(val, s[j])) {
				was_crash = ERROR;
				break;
			}
		}
		else {
			if(val->dataSize > 0){
				if (ERROR == append_string(val, '\0')) {
					was_crash = ERROR;
					break;
				}
				//Что за слово собрали?

				//1) Переменная?
				int index = find_string_in_array(val->data, (const char**)var_list->str, 
																	var_list->dataSize);
				if (index >= 0) {
					if (ERROR == append_string(output, (char)var_list->values[index] + '0')) {
						was_crash = ERROR;
						break;
					}
				} 
				//2) Константа?
				else if (strcmp(BOOL_CONSTANTS[0], val->data) == 0) {
					if (ERROR == append_string(output, '0')) {
						was_crash = ERROR;
						break;
					}
				}
				else if (strcmp(BOOL_CONSTANTS[1], val->data) == 0) {
					if (ERROR == append_string(output, '1')) {
						was_crash = ERROR;
						break;
					}
				}
				//3) Оператор (буквенный)?
				else {
					index = find_string_in_array(val->data, operatorNamesList, 
															operatorsCount);
					if (index < 0) {
						was_crash = ERROR;
						break;
					}
					//обрабатываем оператор
					char oper = operatorShortcuts[index];
					char temp = pop(stack);
					if (!temp) {
						was_crash = ERROR;
						break;
					}
					int tempIndex = (int)(strchr(operList, temp) - operList);
					index = (int)(strchr(operList, oper) - operList);

					while (operRates[index] <= operRates[tempIndex]) {
						was_crash = append_string(output, temp);
						if (ERROR == was_crash) break;
						temp = pop(stack);
						if (!temp) {
							was_crash = ERROR;
							break;
						}
						tempIndex = (int)(strchr(operList, temp) - operList);
					}
					if (ERROR == was_crash) break;
					was_crash = append_string(stack, temp);
					was_crash = append_string(stack, oper);
					if (ERROR == was_crash) break;
				}
				val->dataSize = 0;
			}
			//что за символ попался?
			if (s[j] == '(') {
				was_crash = append_string(stack, s[j]);
			}
			else if (s[j] == ')') {
				char temp;
				while (temp = pop(stack), temp!= '\0' && temp != '(') {
					was_crash = append_string(output, temp);
				}
				if (!temp || ERROR == was_crash) {
					was_crash = ERROR;
					break;
				}
			}
			else if (s[j] != ' ' && s[j] != '\0') {
				was_crash = ERROR;
				break;
			}
			//если пробел - идём дальше
		}
	}
	if (ERROR == was_crash) {
		free_dynamic_string(stack);
		free_dynamic_string(output);
		free_dynamic_string(val);
		return ERROR;
	}
	if (ERROR == free_dynamic_string(val)) {
		free_dynamic_string(stack);
		free_dynamic_string(output);
		return ERROR;
	}
	val = NULL;
	while (stack->dataSize > 1) {
		//в стеке должны остаться только операторы
		char temp = pop(stack);
		if (!temp) {
			was_crash = ERROR;
			break;
		}
		//нижеперечисленных символов быть не должно
		else if (temp == '(' || temp == ')' || is_number(temp) == 1) {
			was_crash = ERROR;
			break;
		}
		was_crash = append_string(output, temp);
		if (ERROR == was_crash) break;
	}
	if (was_crash == OK) {
		*back_poland_notation = output;
	}
	else {
		free_dynamic_string(output);
	}
	if (free_dynamic_string(stack) == ERROR) {
		was_crash = ERROR;
	}
	return was_crash;
}

int calculate_expression(DynString* const s) {
	if (!s) { return ERROR; }

	DynString* stack = init_dynamic_string();
	int was_crash = OK;
	for (unsigned int i = 0; i < s->dataSize; i++) {
		if (s->data[i] >= '0' && s->data[i] <= '9') {
			was_crash = append_string(stack, s->data[i]);
			if (ERROR == was_crash) break;
		}
		else {
			if (stack->dataSize == 0) {
				was_crash = ERROR;
				break;
			}
			char a = pop(stack) - '0';
			if (s->data[i] == '~') {
				was_crash = append_string(stack, '0' + (!a));
				if (was_crash) break;
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
					was_crash = ERROR;
				}
				if (ERROR == was_crash) break;
				append_string(stack, '0' + a);
			}
		}
	}
	int ans = ERROR;
	if (OK == was_crash) {
		ans = pop(stack) - '0';
		if (stack->dataSize != 0 || (ans < 0 || ans > 1)) {
			ans = ERROR;
		}
	}
	free_dynamic_string(stack);
	return ans;
}

//-----Dynamic Array Implementation-----------
int append_string(DynString* const buf, const char data) {
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

int append_dict(DynDict* const buf, char* const str, const int val) {
	if (!buf) { return ERROR; }
	const int strSizeFactor = 2;
	if ((buf->dataSize + 1) > buf->spaceSize) {		//current + zero-symbol
		size_t newBufferSize = buf->spaceSize ? buf->spaceSize*strSizeFactor : 1;
		char** tempStr = (char**)realloc(buf->str, newBufferSize * sizeof(char*));
		int* tempInt = (int*)realloc(buf->values, newBufferSize * sizeof(int));
		if (!tempStr || !tempInt) {
			free_dynamic_dict(buf);
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

int free_dynamic_string(DynString* const stack) {
	if (!stack) { return ERROR; }
	if(stack->data)
		free(stack->data);
	free(stack);
	return OK;
}

int free_dynamic_dict(DynDict* const dict) {
	if (!dict) { return ERROR; }
	if (dict->str) {
		for (unsigned int i = 0; i < dict->dataSize; i++) {
			free(dict->str[i]);
		}
		free(dict->str);
	}
	if(dict->values)
		free(dict->values);
	free(dict);
	return OK;
}

int trunc_dynamic_string(DynString* const dynamic_array) {
	if (!dynamic_array) { return ERROR; }
	if (dynamic_array->dataSize < dynamic_array->spaceSize) {
		char* temp = (char*)realloc(dynamic_array->data, dynamic_array->dataSize * sizeof(char));
		if (!temp && dynamic_array->dataSize != 0) {
			return ERROR;
		}
		dynamic_array->data = temp;
		dynamic_array->spaceSize = dynamic_array->dataSize;
		return OK;
	}
	return OK;
}

DynString* init_dynamic_string(void) {
	DynString* arr = (DynString*)malloc(sizeof(DynString));
	if (!arr) { return NULL; }
	arr->data = NULL;
	arr->dataSize = 0;
	arr->spaceSize = 0;
	return arr;
}

DynDict* init_dynamic_dict(void) {
	DynDict* arr = (DynDict*)malloc(sizeof(DynDict));
	if (!arr) { return NULL; }
	arr->str = NULL;
	arr->values = NULL;
	arr->dataSize = 0;
	arr->spaceSize = 0;
	return arr;
}

int is_letter(const char c) {
	if (c >= 'A' && c <= 'z') {
		return 1;
	}
	else {
		return 0;
	}
}

int is_number(const char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	}
	else {
		return 0;
	}
}

int find_string_in_array(const char* const what, const char** where, const int where_len) {
	if (where_len <= 0) { return ERROR; }
	if (!what || !where || !(*where)) { return ERROR; }

	for (int j = 0; j < where_len; j++) {
		if (strcmp(what, where[j]) == 0) {
			return j;
		}
	}
	return ERROR;
}
