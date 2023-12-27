#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//define sizes 
#define TRUE 1
#define FALSE 0
#define LENGTH_WORD 33
#define LENGTH_LABEL 51 
#define LENGTH_LINE 501
#define MAX_ADDRESS 512
#define MAX_INSTRUCTIONS 512

//define structs
typedef struct  _Label
{
	char name[LENGTH_LABEL];
	int address;
}Label;

//define structs                             
typedef struct  _mem
{
	char value[LENGTH_WORD];
	int address;
}mem;

typedef struct _Instruction
{
	char opcode[9];
	char rd[9];
	char rs[9];
	char rt[9];
	char imm[LENGTH_LABEL];
}Instruction;

//declare functions 
int save_labels_to_array(FILE* file, Label* labels_array);
void ignoreComment(char* str);
void removeWhiteSpaceAtBeginning(char* str);
int isBlank(char* str);
void removeWhiteSpaceAtEnd(char* str);
void read_line(Instruction* inst_array, mem* mem_array, char* tok, int* inst_counter, int* data_counter);
void save_word(mem* mem_array, int data_counter, char* tok);
void initialize_word(mem* mem_array, int data_counter, int address, char* data_str);
void save_instruction(Instruction* inst_array, int inst_counter, int field, char* tok);
void change_imms_labels(Instruction* inst_array, Label* label_array, int inst_counter, int label_counter);
void print_inst_content(Instruction* inst_array, FILE* fp, int inst_counter);
void print_regs(char* reg, FILE* fp);
void print_opcode(char* opcode, FILE* fp);
void print_imm(char* imm, FILE* fp);
void print_data_content(mem* mem_array, FILE* fp, int data_counter, int inst_counter);
int max_adress(mem* mem_array, int data_counter);
int strcmpUntilWhitespace(const char* str1, const char* str2);

//main function
int main(int argc, char* argv[])
{

	int  inst_counter = 0, label_counter = 0, i = 0, data_counter = 0;
	FILE* program_file = NULL, * memin_file = NULL;
	Label labels_array[MAX_INSTRUCTIONS];
	Instruction inst_array[MAX_INSTRUCTIONS];
	mem mem_array[MAX_ADDRESS];

	//first run on the assembly code - to get the labels from the file into array of labels
	program_file = fopen(argv[1], "r");
	if (program_file == NULL)
	{
		printf("Error1: couldn't open file, exit program\n");
		exit(1);
	}

	//save each label and it's address to 'labels_array'.
	//save the number of labels to 'num_labels'
	label_counter = save_labels_to_array(program_file, labels_array);
	fclose(program_file);


	//second run on the assembly file - save instruction to inst_array and data to mem_array.
	// 'inst_counter' save number of instructions, 'data_counter' save number of .word instruction.
	program_file = fopen(argv[1], "r");
	if (program_file == NULL)
	{
		printf("Error2: couldn't open file, exit program\n");
		exit(1);
	}
	//the array of instructions saves all the instruction arguments and mem_array saves all words from .word instruction.
	inst_counter = read_file(program_file, inst_array, mem_array, &data_counter);
	fclose(program_file);

	//in case the imm field contains a label, cahnge the label from it's name to it's address.
	change_imms_labels(inst_array, labels_array, inst_counter, label_counter);

	//printing the instructions to  memin_file
	memin_file = fopen(argv[2], "w");
	if (memin_file == NULL)
	{
		printf("Error3: couldn't open file, exit program\n");
		exit(1);
	}
	print_inst_content(inst_array, memin_file, inst_counter);
	fclose(memin_file);

	//adding the memory content to memin file
	memin_file = fopen(argv[2], "a");
	if (memin_file == NULL)
	{
		printf("Error4: couldn't open file, exit program\n");
		exit(1);
	}
	print_data_content(mem_array, memin_file, data_counter, inst_counter);
	fclose(memin_file);

	return 0;
}


// Save name and address of the labels from the assembly file to 'labels_array'
int save_labels_to_array(FILE* file, Label* labels_array) {
	int labelCount = 0;   // Counter for the number of labels
	int lineCount = 0;    // Counter for the current line number
	char line[LENGTH_LINE];
	char* labelMarker;
	char* labelName;
	char* remainingLine;

	// Read each line from the file
	while (fgets(line, LENGTH_LINE, file)) {
		// Preprocess the line
		removeWhiteSpaceAtBeginning(line);
		ignoreComment(line);

		// Skip blank lines
		if (isBlank(line))
			continue;

		// Check for a label marker (":") in the line
		labelMarker = strchr(line, ':');
		if (labelMarker != NULL) {
			// Extract label name
			labelName = strtok(line, ":");
			// Save label information in the labels_array
			strcpy(labels_array[labelCount].name, labelName);
			labels_array[labelCount].address = lineCount;
			labelCount++;

			// Check if there is a command in the same line as the label
			remainingLine = strtok(NULL, ":");
			if (!isBlank(remainingLine) && strstr(remainingLine, ".word") == NULL)
				lineCount++;
		}
		else {
			// No label marker (":") in the line
			// Check if it's not a ".word" line, then increase lineCount
			if (strstr(line, ".word") == NULL)
				lineCount++;
		}
	}

	return labelCount;  // Return the total number of labels found
}

// Function to ignore comments in a string
void ignoreComment(char* str) {
	char* comment = strchr(str, '#');
	if (comment != NULL)
		*comment = '\0';  // Nullify the string at the position of the comment
}


// Function to remove leading white spaces from a string
void removeWhiteSpaceAtBeginning(char* str) {
	int source_i, dest_i;
	char str_without_spaces[LENGTH_LINE];

	// Find the first non-space character index
	for (source_i = 0; isspace(str[source_i]); source_i++);

	// Check if the string is blank
	if (source_i == strlen(str)) {
		*str = '\0';  // If blank, set the string to an empty string
		return;
	}

	// Create a new string without leading spaces
	for (dest_i = 0; source_i < strlen(str); source_i++) {
		str_without_spaces[dest_i] = str[source_i];
		dest_i++;
	}

	str_without_spaces[dest_i] = '\0';
	strncpy(str, str_without_spaces, strlen(str));
}

// Function to check if a string is blank
int isBlank(char* str) {
	for (int char_i = 0; char_i < strlen(str); char_i++) {
		if (!isspace(str[char_i])) {
			return FALSE;  // If any non-space character found, the string is not blank
		}
	}
	return TRUE;  // String is blank
}

//goes through the file and saves the arguments of the instructions and the pseudointruction.
int read_file(FILE* file, Instruction* inst_array, mem* mem_array, int* data_counter)
{
	char line[LENGTH_LINE];
	char* labelMarker;
	int inst_counter = 0;
	char* remainingLine;
	char* tok;
	while (fgets(line, LENGTH_LINE, file))
	{
		// Preprocess the line
		removeWhiteSpaceAtBeginning(line);
		ignoreComment(line);

		// Skip blank lines
		if (isBlank(line))
			continue;
		// search for labels in the line 
		labelMarker = strchr(line, ':');
		//there is a ":" in the line
		if (labelMarker != NULL)
		{
			// get line without the label
			remainingLine = labelMarker + 1;

			//checks if there is a command after label 
			if (!isBlank(remainingLine))
			{
				tok = strtok(remainingLine, " ,");
				// save line in correct data structure
				read_line(inst_array, mem_array, tok, &inst_counter, data_counter);
			}

		}
		else
		{
			tok = strtok(line, " ,");
			// save line in correct data structure
			read_line(inst_array, mem_array, tok, &inst_counter, data_counter);
			
	
		}
	}
	return inst_counter;
}

// gets a line from the file and saves the instruction/pseudointruction in the correct fields of struct
void read_line(Instruction* inst_array,mem* mem_array,char* tok,int* inst_counter,int* data_counter)
{
	int field_num = 0;
	// check if line contains an instruction or a pseudointruction 
	if (strcmp(tok, ".word") != 0)
	{
		// an instruction 
		while (tok != NULL)
		{
			save_instruction(inst_array, (*inst_counter), field_num, tok);
			field_num++;
			tok = strtok(NULL, " ,");
		}
		(*inst_counter)++;

	}
	else
	{
		// a pseudointruction
		save_word(mem_array, (*data_counter), tok);
		(*data_counter)++;

	}
}




// takes paramters from .word pseudointruction and saves it to mem_array
void save_word(mem* mem_array, int data_counter, char* tok)
{
	int i = 0;
	char data_str[LENGTH_LINE];
	int address = 0, data = 0;
	while (tok != NULL)
	{
		//if it is an address
		if (i == 1)
		{
			//converts from string to an integer (if address hex or dec)
			address = strtoll(tok, NULL, 0);
		}
		//save the data
		else if (i == 2)
		{
			//converts string to int
			data = strtoll(tok, NULL, 0);
			sprintf(data_str, "%d", data);

		}
		tok = strtok(NULL, " ,");
		i++;
	}
	//sets data in the new address and updates array
	initialize_word(mem_array, data_counter, address, data_str);
}

// saves the data and the address in the struct fields
void initialize_word(mem* mem_array, int data_counter, int address, char* data_str)
{
	if (address < 0)
		address *= -1;
	if (address >= 512)
	{
		address = address % 512;
	}
	mem_array[data_counter].address = address;
	strcpy(mem_array[data_counter].value, data_str);
}

// saves the instructions arguments in their fields in the struct
void save_instruction(Instruction* inst_array, int inst_counter, int field, char* tok)
{
	switch (field)
	{
	case 0:
		
		strcpy(inst_array[inst_counter].opcode, tok);
		break;
	case 1:
		
		strcpy(inst_array[inst_counter].rd, tok);
		break;
	case 2:

		strcpy(inst_array[inst_counter].rs, tok);
		break;
	case 3:

		strcpy(inst_array[inst_counter].rt, tok);
		break;
	case 4:
		removeWhiteSpaceAtEnd(tok);
		strcpy(inst_array[inst_counter].imm, tok);
		break;

	default:
		break;
	}

}
void removeWhiteSpaceAtEnd(char* str)
{
	while (!isspace(*str))
		str++;
	*str = '\0';
}


// changes imm fields that contains the label name to the label's address
void change_imms_labels(Instruction* inst_array, Label* label_array, int inst_counter, int label_counter)
{
	char address_str[MAX_ADDRESS];
	int inst_i;
	int label_i;

	for (inst_i = 0; inst_i < inst_counter; inst_i++) {
		//checks if the imm field contains a name of a label
		if (isalpha(inst_array[inst_i].imm[0])) {
			for (label_i = 0; label_i < label_counter; label_i++) {
				//search for lable name in the labels array
				if (strcmpUntilWhitespace(label_array[label_i].name, inst_array[inst_i].imm) == 0) {
					//gets the address of the label and changes the imm field to the adress of the label
					sprintf(address_str, "%d", label_array[label_i].address);
					strcpy(inst_array[inst_i].imm, address_str);
				}
			}
		}

	}

}

// prints intructions from the inst_array  to the memin file as an 8 digits hexadecimal
void print_inst_content(Instruction* inst_array, FILE* fp, int inst_counter)
{
	int imm_num = 0;
	char hex[8];
	int i;

	for (i = 0; i < inst_counter; i++)
	{
		//prints a line in the file
		print_opcode(inst_array[i].opcode, fp);
		print_regs(inst_array[i].rd, fp);
		print_regs(inst_array[i].rs, fp);
		print_regs(inst_array[i].rt, fp);
		print_imm(inst_array[i].imm, fp);
		fprintf(fp, "\n");
	}
}

// prints the opcode to memin file in hex
void print_opcode(char* opcode, FILE* fp)
{
	if (strstr(opcode, "add") != NULL)
		fprintf(fp, "00");
	else if (strstr(opcode, "sub") != NULL)
		fprintf(fp, "01");
	else if (strstr(opcode, "and") != NULL)
		fprintf(fp, "02");
	else if (strstr(opcode, "or") != NULL)
		fprintf(fp, "03");
	else if (strstr(opcode, "sll") != NULL)
		fprintf(fp, "04");
	else if (strstr(opcode, "sra") != NULL)
		fprintf(fp, "05");
	else if (strstr(opcode, "srl") != NULL)
		fprintf(fp, "06");
	else if (strstr(opcode, "beq") != NULL)
		fprintf(fp, "07");
	else if (strstr(opcode, "bne") != NULL)
		fprintf(fp, "08");
	else if (strstr(opcode, "blt") != NULL)
		fprintf(fp, "09");
	else if (strstr(opcode, "bgt") != NULL)
		fprintf(fp, "0A");
	else if (strstr(opcode, "ble") != NULL)
		fprintf(fp, "0B");
	else if (strstr(opcode, "bge") != NULL)
		fprintf(fp, "0C");
	else if (strstr(opcode, "jal") != NULL)
		fprintf(fp, "0D");
	else if (strstr(opcode, "lw") != NULL)
		fprintf(fp, "0E");
	else if (strstr(opcode, "sw") != NULL)
		fprintf(fp, "0F");
	else if (strstr(opcode, "reti") != NULL)
		fprintf(fp, "10");
	else if (strstr(opcode, "in") != NULL)
		fprintf(fp, "11");
	else if (strstr(opcode, "out") != NULL)
		fprintf(fp, "12");
	else if (strstr(opcode, "halt") != NULL)
		fprintf(fp, "13");
	else
		fprintf(fp, "00");

}

//prints registers to memin file in hex
void print_regs(char* reg, FILE* fp)
{
	if (strcmp(reg, "$zero") == 0)
		fprintf(fp, "0");
	else if (strcmp(reg, "$imm") == 0)
		fprintf(fp, "1");
	else if (strcmp(reg, "$v0") == 0)
		fprintf(fp, "2");
	else if (strcmp(reg, "$a0") == 0)
		fprintf(fp, "3");
	else if (strcmp(reg, "$a1") == 0)
		fprintf(fp, "4");
	else if (strcmp(reg, "$t0") == 0)
		fprintf(fp, "5");
	else if (strcmp(reg, "$t1") == 0)
		fprintf(fp, "6");
	else if (strcmp(reg, "$t2") == 0)
		fprintf(fp, "7");
	else if (strcmp(reg, "$t3") == 0)
		fprintf(fp, "8");
	else if (strcmp(reg, "$s0") == 0)
		fprintf(fp, "9");
	else if (strcmp(reg, "$s1") == 0)
		fprintf(fp, "A");
	else if (strcmp(reg, "$s2") == 0)
		fprintf(fp, "B");
	else if (strcmp(reg, "$gp") == 0)
		fprintf(fp, "C");
	else if (strcmp(reg, "$sp") == 0)
		fprintf(fp, "D");
	else if (strcmp(reg, "$fp") == 0)
		fprintf(fp, "E");
	else if (strcmp(reg, "$ra") == 0)
		fprintf(fp, "F");
	else
		fprintf(fp, "0");
}

//prints imm to the memin file and translates it to hexadecimal number
void print_imm(char* imm, FILE* fp)
{
	int imm_num = 0;
	char hex[8];
	//converts string to int
	imm_num = strtoll(imm, NULL, 0);
	//converts to 3 digits hexadecimal number
	sprintf(&hex, "%03X", imm_num & 0xfff);
	fprintf(fp, "%s", hex);
}

// add the data content to the memin file
void print_data_content(mem* mem_array, FILE* fp, int data_counter, int inst_counter)
{
	int flag = 0;
	int max_adr = max_adress(mem_array, data_counter);
	int val_num = 0;
	char hex[9];
	for (int i = inst_counter; i <= max_adr; i++)
	{
		flag = 0;
		for (int j = 0; j < data_counter&& flag == 0; j++)
		{
			if (mem_array[j].address == i) // a word from mem_array need to be printed in this location 
			{
				flag = 1;
				val_num = strtoll(mem_array[j].value, NULL, 0); // convert from string to int 
				sprintf(&hex, "%08X", val_num & 0xffffffff); // convert to 8 hex digits
				fprintf(fp, "%s\n", hex); // print in memin file 

			}
		}
		if (flag == 0) // If neither instruction nor data needs to be printed in this location, print zeros.
			fprintf(fp, "%s\n", "00000000");
	}

}
// finds the word from mem_array with max adress
int max_adress(mem* mem_array, int data_counter)
{
	int max = 0;
	for (int i = 0; i < data_counter; i++)
	{
		if (mem_array[i].address > max)
		{
			max = mem_array[i].address;
		}
	}
	return max;
}

int strcmpUntilWhitespace(const char* str1, const char* str2) {
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0' && !isspace(str1[i]) && !isspace(str2[i])) {
		if (str1[i] < str2[i]) {
			return -1;
		}
		else if (str1[i] > str2[i]) {
			return 1;
		}
		i++;
	}

	// Strings are equal until a whitespace or end of string is encountered
	if ((str1[i] == '\0' || isspace(str1[i])) && (str2[i] == '\0' || isspace(str2[i]))) {
		return 0;  // Strings are equal
	}
	else if (str1[i] == '\0' || isspace(str1[i])) {
		return -1; // str1 is shorter or has encountered a whitespace earlier
	}
	else {
		return 1;  // str2 is shorter or has encountered a whitespace earlier
	}
}
