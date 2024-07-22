#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 128


//MAX NUMBERS OF LINES IN .S MUST BE <= 2^31 lines
/*RISC-V Instructions*/
char IRII[36][5] = {"ADDI", "ANDI","ORI","XORI","SLTI","NOP",
                    "SLLI","SRLI","SRAI","LUI","AUIPC","ADD",
                    "SLT","SLTU","AND","OR","XOR","SLL",
                    "SRL","SUB","SRA","JAL","JALR","BEQ",
                    "BNE","BLT","BGE","LW","LH","LHU",
                    "LB","LBU","SW","SH","SB"};


//Headers
int file_number_lines(char asm_file_input_name[32]);
void dec_To_Binary(int dec_input, int* binary_output, int sizeOfArray);
void save_labels_address(int file_length_input, char asm_file_input_name[32], char labels_input[file_length_input][32], int* labels_mem_address_input, int* input_num_labels);
int check_label_exists(char buffer_instruction_input[BUFFER_SIZE]);
int return_labels_memory_address(int file_length_input, char buffer_instruction_input[BUFFER_SIZE], char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels);
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5]);
void IRRI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5]);
void CTI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5], int file_length_input, char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels);
void CBI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6], int file_length_input, char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels);
void LI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6]);
void SI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6]);

int main(void) {

    char ASM_File_Name[32];
    char Machine_File_Name[48] = "";
    int file_length;
    int current_line = 1;
    int dec_instruct_address = 0;
    char buffer[BUFFER_SIZE];
    char prev_buffer[BUFFER_SIZE];
    char buffer_instruction[6];

    /*/////////////////////
    //   Read from FILE  //
    *//////////////////////
    printf("Please input file name(including file type): ");
    scanf("%s", &ASM_File_Name);

    strncpy(Machine_File_Name, ASM_File_Name, strlen(ASM_File_Name) - 2);
    strcat(Machine_File_Name, "_machine.txt\0");

    FILE *ASM_File, *Machine_File;
    if((ASM_File = fopen(ASM_File_Name, "r")) == NULL)
    {
        printf("ERROR: Could Not Open File");
        return 0;
    }
    fclose(ASM_File); //Confirmed file exists (Will now open only when needed)
    file_length = file_number_lines(ASM_File_Name);

    
    char labels[file_length][32]; //labels shall not be more than 32 characters in size including ':' and '.' 
    int labels_mem_address[file_length];
    int* number_of_labels;

    //ITERATE THROUGH THE ENTIRE FILE AND CAPTURE THE MEMORY ADDRESS OF ALL LABELS (FOR JUMPS) [PASS 1]
    save_labels_address(file_length, ASM_File_Name, labels, &labels_mem_address, &number_of_labels);

    if(file_length > 0) //[PASS 2]
    {
        ASM_File = fopen(ASM_File_Name, "r");
        Machine_File = fopen(Machine_File_Name, "w");
        
        while(current_line <= file_length)
        {   
            //Detect instruction
            fgets(buffer, BUFFER_SIZE, ASM_File);

            int k = 0;
            while(strncmp(buffer + k, " ", 1) == 0)
            {
                k++;
            }
            strncpy(buffer_instruction, buffer + k, 5);
            strcat(buffer_instruction, "\0");
            //IRII (I-Type Instructions)
            if(strncmp(buffer_instruction, "ADDI", 4) == 0 || strncmp(buffer_instruction, "ANDI", 4) == 0 || strncmp(buffer_instruction, "ORI", 3) == 0 ||
               strncmp(buffer_instruction, "XORI", 4) == 0 || strncmp(buffer_instruction, "SLTI", 4) == 0 || strncmp(buffer_instruction, "NOP", 3) == 0 ||
               strncmp(buffer_instruction, "SLLI", 4) == 0 || strncmp(buffer_instruction, "SRLI", 4) == 0 || strncmp(buffer_instruction, "SRAI", 4) == 0 ||
               strncmp(buffer_instruction, "LUI", 3) == 0  || strncmp(buffer_instruction, "AUIPC", 5) == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            //IRRI (R-Type Instructions)
            else if(strncmp(buffer_instruction, "ADD", 3) == 0 || strncmp(buffer_instruction, "SLT", 3) == 0 || strncmp(buffer_instruction, "SLTU", 4) == 0 ||
                    strncmp(buffer_instruction, "AND", 3) == 0 || strncmp(buffer_instruction, "OR", 2) == 0 || strncmp(buffer_instruction, "XOR", 3 ) == 0 ||
                    strncmp(buffer_instruction, "SLL", 3) == 0 || strncmp(buffer_instruction, "SRL", 3) == 0 || strncmp(buffer_instruction, "SUB", 3) == 0 ||
                    strncmp(buffer_instruction, "SRA", 3) == 0)
            {
                IRRI_Handle(buffer, Machine_File, buffer_instruction);
            }
            //Control Transfer Instructions (J-Type Instructions)
            else if(strncmp(buffer_instruction, "JAL", 3) == 0 || strncmp(buffer_instruction, "JALR", 4) == 0)
            {
                CTI_Handle(buffer, Machine_File, buffer_instruction, file_length, labels, labels_mem_address, number_of_labels);
            }
            //Conditional Branches (B-Type Instructions)
            else if(strncmp(buffer_instruction, "BEQ", 3) == 0 || strncmp(buffer_instruction, "BNE", 3) == 0 ||
                    strncmp(buffer_instruction, "BLT", 3) == 0 || strncmp(buffer_instruction, "BGE", 3) == 0) 
            {

            }
            //Load Instructions (I-Type Instructions)
            else if(strncmp(buffer_instruction, "LW", 2) == 0 || strncmp(buffer_instruction, "LH", 2) == 0 || strncmp(buffer_instruction, "LHU", 3) == 0 ||
                    strncmp(buffer_instruction, "LB", 2) == 0 || strncmp(buffer_instruction, "LBU", 3) == 0)
            {

            }
            //Store Instructions (S-Type Instructions)
            else if(strncmp(buffer_instruction, "SW", 2) == 0 || strncmp(buffer_instruction, "SH", 2) == 0 || strncmp(buffer_instruction, "SB", 2) == 0 )
            {

            }
            else if(check_label_exists(buffer) == 0)
            {
                printf("ERROR: Unrecognized instruction at LINE %d\n", current_line);
                break;
            }

            //now for calculating memory address of instruction

            if(current_line > 1 && check_label_exists(buffer) == 1 && check_label_exists(prev_buffer) == 0)
            {
                //add 4 to memory address if previous line isn't a label but current line is
                //Ex.   ADD R3, #4
                ///   start:
                dec_instruct_address += 4;
            }
            else if(current_line > 1 && check_label_exists(buffer) == 1 && check_label_exists(prev_buffer) == 1)
            {
                //don't add to memory address if current line and previous line are labels
                //Ex. start1:
                //    start2:
            }
            else if(current_line > 1 && check_label_exists(buffer) == 0 && check_label_exists(prev_buffer) == 1)
            {
                //don't add to memory address if current line is not a label but previous line is
                //Ex. start1:
                //      ADD R3, #4
            }
            else if(current_line > 1 && check_label_exists(buffer) == 0 && check_label_exists(prev_buffer) == 0)
            {
                //if previous line and current line are not labels then increment memory address (it is instructions)
                dec_instruct_address += 4;
            }
            printf("@%.4d %s\n", dec_instruct_address, buffer);
            strcpy(prev_buffer,buffer);
            current_line++;
        }
    }

    printf("\n\nFile processing finished\n\n");
    fclose(ASM_File);
    fclose(Machine_File);
    
}

int file_number_lines(char asm_file_input_name[32])
{
    FILE* ASM_File_Input = fopen(asm_file_input_name, "r");
    int counter = 0;
    char buffer[BUFFER_SIZE];
    while(fgets(buffer, BUFFER_SIZE, ASM_File_Input) != NULL)
    {
        counter++;
    }
    fclose(ASM_File_Input);

    return counter;
}

void dec_To_Binary(int dec_input, int* binary_output, int sizeOfArray)
{
    int current_dec = dec_input;
    int decimal_place = 0;
    int output = 0;
    int x = sizeOfArray;
    while(current_dec > 0 || x > 0)
    {
        if(current_dec > 0)
        {
            *(binary_output + (sizeOfArray - 1) - decimal_place) = (current_dec % 2);
            current_dec /= 2;
            decimal_place++;
            x--;
        }
        else
        {
            *(binary_output + (sizeOfArray - 1) - decimal_place) = 0;
            decimal_place++;
            x--;
        }
    }
}

void save_labels_address(int file_length_input, char asm_file_input_name[32], char labels_input[file_length_input][32], int* labels_mem_address_input, int* input_num_labels)
{
    FILE *ASM_File_Input; 
    ASM_File_Input = fopen(asm_file_input_name, "r");
    int current_line = 1;
    char buffer[BUFFER_SIZE];
    char prev_buffer[BUFFER_SIZE];
    int index = 0;
    int mem_address = 0;
    *input_num_labels = 0;
    while(current_line <= file_length_input)
    {
        fgets(buffer, BUFFER_SIZE, ASM_File_Input);
        if(strstr(buffer,":") != NULL)
        {
            *input_num_labels += 1;
            if(current_line > 1)
            {
                //checks if previous line is also a label (therefore memory address is the same)
                if(strstr(prev_buffer, ":") != NULL)
                {
                    labels_mem_address_input[index] = labels_mem_address_input[index - 1];
                }
                else
                {
                    mem_address += 4;
                    labels_mem_address_input[index] = mem_address;
                }
                strcpy(labels_input[0] + 64*index,buffer);
                //strcpy(labels_input[0] + 64*index + (i - 1), "\0");
            }
            else
            {
                strcpy(labels_input[0],buffer);
                labels_mem_address_input[0] = 0;
            }
            //printf("INDEX:%d %d\n", index, labels_mem_address_input[index]);
            index++;
        }
        else if(current_line > 1)
        {
            //if previous line and current line do not have a label then add 4 to mem address
            if(strstr(prev_buffer, ":") == NULL)
            {
                mem_address += 4;
            }
        }
        strcpy(prev_buffer,buffer);
        current_line++;
    }
    fclose(ASM_File_Input);
}

//if label exists then return 1 else return 0
int check_label_exists(char buffer_instruction_input[BUFFER_SIZE])
{
    if(strstr(buffer_instruction_input, ":") != NULL)
    {
        return 1;
    }
    return 0;
}

//returns the memory address of a label
int return_labels_memory_address(int file_length_input, char buffer_instruction_input[BUFFER_SIZE], char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels)
{
    //printf("%s", buffer_instruction_input);
    for(int i = 0; i < number_of_labels; i++)
    {
        if(strstr(labels_input[0] + 64*i,buffer_instruction_input) != NULL)
        {
            return labels_mem_address_input[i];
        }
    }
    printf("ERROR: Segmentation Fault, Label Does Not Exist\n");
    return -1;
}

//Integer Register-Immediate Instructions Handler
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6])
{
    char buffer_machine[33];
    if(strncmp(instruction_header,"ADDI", 4) == 0 || strncmp(instruction_header,"ANDI", 4) == 0 || strncmp(instruction_header,"ORI", 3) == 0 || strncmp(instruction_header,"XORI", 4) == 0 || 
       strncmp(instruction_header,"SLTI", 4) == 0)
       {
            //Input Imm[11:0] (BITS [31:20])
            int imm[12];
            char imm_string[5];
            strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 4);
            strcat(imm_string, "\0");
            dec_To_Binary(atoi(imm_string), &imm, 12);
            for(int i = 0; i < 12; i++)
            {
                buffer_machine[i] = imm[i] +'0';
            }

            //Input RS1 (BITS [19:15])
            int rs1[5];
            char rs1_string[3];
            strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
            strcat(rs1_string, "\0");
            dec_To_Binary(atoi(rs1_string), &rs1, 5);

            for(int i = 0; i < 5; i++)
            {

                buffer_machine[i + 12] = rs1[i] +'0';
            }

            //Input RD (BITS [11:7])
            int rd[5];
            char rd_string[3];
            strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
            strcat(rd_string, "\0");
            dec_To_Binary(atoi(rd_string), &rd, 5);
            for(int i = 0; i < 5; i++)
            {
                buffer_machine[20 + i] = rd[i] +'0';
            }

            if(strncmp(instruction_header,"ADDI", 4) == 0)
            {
                //Ex. ADDI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    ADDI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '0';
                buffer_machine[18] = '0';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';

            }
            else if(strncmp(instruction_header, "ANDI", 4) == 0)
            {
                //Ex. ANDI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    ANDI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '1';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
            else if(strncmp(instruction_header, "ORI", 3) == 0)
            {
                //Ex. ORI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    ORI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '1';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
            else if(strncmp(instruction_header, "XORI", 4) == 0)
            {
                //Ex. XORI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    XORI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '0';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
            else if(strncmp(instruction_header, "SLTI", 4) == 0)
            {
                //Ex. SLTI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    SLTI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '0';
                buffer_machine[18] = '1';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
    }
    else if(strncpy(instruction_header, "NOP", 3) == 0)
    {
        for(int i = 0; i < 25; i++)
        {
            buffer_machine[i] = '0';
        }

        //opcode
        buffer_machine[25] = '0';
        buffer_machine[26] = '0';
        buffer_machine[27] = '1';
        buffer_machine[28] = '0';
        buffer_machine[29] = '0';
        buffer_machine[30] = '1';
        buffer_machine[31] = '1';
    }
    else if(strncpy(instruction_header, "SLLI", 4) == 0 || strncpy(instruction_header, "SRLI", 4) == 0 || strncpy(instruction_header, "SRAI", 4))
    {
        //Input Imm[4:0] (BITS [24:20])
        int imm[5];
        char imm_string[3];
        strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 2);
        strcat(imm_string, "\0");
        dec_To_Binary(atoi(imm_string), &imm, 5);
        for(int i = 0; i < 5; i++)
        {

            buffer_machine[i + 7] = imm[i] +'0';
        }

        //Input RS1 (BITS [19:15])
            int rs1[5];
            char rs1_string[3];
            strncpy(rs1_string,strstr(buffer_instruction_input, ",R") + 2, 2);
            strcat(rs1_string, "\0");
            dec_To_Binary(atoi(rs1_string), &rs1, 5);
            for(int i = 0; i < 5; i++)
            {
                buffer_machine[i + 12] = rs1[i] +'0';
            }

            //Input RD (BITS [11:7])
            int rd[5];
            char rd_string[3];
            strncpy(rd_string,strstr(buffer_instruction_input, " R") + 2, 2);
            strcat(rd_string, "\0");
            dec_To_Binary(atoi(rd_string), &rd, 5);
            for(int i = 0; i < 5; i++)
            {
                buffer_machine[20 + i] = rd[i] +'0';
            }

            //imm[11:5] BITS[31:25]
            buffer_machine[0] = 0;
            buffer_machine[1] = 0;
            buffer_machine[2] = 0;
            buffer_machine[3] = 0;
            buffer_machine[4] = 0;
            buffer_machine[5] = 0;
            buffer_machine[6] = 0;
            if(strncmp(instruction_header, "SRAI", 4) == 0)
            {
                buffer_machine[1] = 1;
            }

            //funct3 BITS[14:12] && OPCODE BITS[6:0]
            if(strncmp(instruction_header, "SLLI", 4) == 0)
            {
                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '0';
                buffer_machine[18] = '0';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
            else if(strncmp(instruction_header, "SRLI", 4) == 0)
            {
                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '0';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
            else if(strncmp(instruction_header, "SRAI", 4) == 0)
            {
                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '0';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '1';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '1';
            }
    }
    else if(strncpy(instruction_header, "LUI", 3) == 0 || strncpy(instruction_header, "AUIPC", 5) == 0)
    {
        //Ex. LUI R4,#imm
        //Ex. AUIPC R5,#imm

        //Input Imm[19:0] (BITS [31:12])
        int imm[20];
        char imm_string[8];
        strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 7);
        strcat(imm_string, "\0");
        dec_To_Binary(atoi(imm_string), &imm, 12);
        for(int i = 0; i < 20; i++)
        {

            buffer_machine[i] = imm[i] +'0';
        }

        //Input RD (BITS [11:7])
        int rd[5];
        char rd_string[3];
        strncpy(rd_string,strstr(buffer_instruction_input, " R") + 2, 2);
        strcat(rd_string, "\0");
        dec_To_Binary(atoi(rd_string), &rd, 5);

        for(int i = 0; i < 5; i++)
        {
            buffer_machine[20 + i] = rd[i] +'0';
        }

        if(strncmp(instruction_header, "LUI", 3) == 0)
        {
            //Input opcode (BITS [6:0])
            buffer_machine[25] = '0';
            buffer_machine[26] = '1';
            buffer_machine[27] = '1';
            buffer_machine[28] = '0';
            buffer_machine[29] = '1';
            buffer_machine[30] = '1';
            buffer_machine[31] = '1';
        }
        else if(strncmp(instruction_header, "AUIPC", 5) == 0)
        {
            //Input opcode (BITS [6:0])
            buffer_machine[25] = '0';
            buffer_machine[26] = '0';
            buffer_machine[27] = '1';
            buffer_machine[28] = '0';
            buffer_machine[29] = '1';
            buffer_machine[30] = '1';
            buffer_machine[31] = '1';
        }

    }

    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine); 
}

//Integer Register-Register Instructions Handler
void IRRI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6])
{
    char buffer_machine[33];

    //Input RS2 (BITS [24:20])
    int rs2[5];
    char rs2_string[3];
    strncpy(rs2_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs2_string, "\0");
    dec_To_Binary(atoi(rs2_string), &rs2, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[i + 7] = rs2[i] +'0';
    }

    //Input RS1 (BITS [19:15])
    int rs1[5];
    char rs1_string[3];
    strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs1_string, "\0");
    dec_To_Binary(atoi(rs1_string), &rs1, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[i + 12] = rs1[i] +'0';
    }

    //Input RD (BITS [11:7])
    int rd[5];
    char rd_string[3];
    strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rd_string, "\0");
    dec_To_Binary(atoi(rd_string), &rd, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[20 + i] = rd[i] +'0';
    }

     //Input opcode (BITS [6:0])
    buffer_machine[25] = '0';
    buffer_machine[26] = '1';
    buffer_machine[27] = '1';
    buffer_machine[28] = '0';
    buffer_machine[29] = '0';
    buffer_machine[30] = '1';
    buffer_machine[31] = '1';

    if(strncmp(instruction_header, "ADD", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header, "SUB", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '1';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header, "SLL", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header, "SLTU", 4) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '1';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header, "SLT", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '1';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"XOR", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }   
    else if(strncmp(instruction_header, "SRL", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header, "SRA", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '1';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header, "OR", 2) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '1';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header, "AND", 3) == 0)
    {
        //funct7 (BITS[31:25])
        buffer_machine[0] = '0';
        buffer_machine[1] = '0';
        buffer_machine[2] = '0';
        buffer_machine[3] = '0';
        buffer_machine[4] = '0';
        buffer_machine[5] = '0';
        buffer_machine[6] = '0';

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '1';
        buffer_machine[19] = '1';
    }
    
    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine); 
}

//Control Transfer Instructions Handler
void CTI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6], int file_length_input, char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels)
{
    //Ex. JAL x7,#imm <-can also be a label
    //Jump to #imm and save PC + 4 to x7
    //Ex. JALR x7,x1,#imm
    //Jump to x1 + #imm and save PC + 4 to x7
    // Ex. JALR x7, 4(x1)
    //Jump to x1 + 4 and save PC + 4 to x7

    char buffer_machine[33];
    char label_input_instruct[32];

    if(strncmp(instruction_header,"JALR", 4) == 0 )
    {
        //Input Imm[11:0] (BITS [31:20])
        int imm[12];
        char imm_string[5];
        if(strstr(buffer_instruction_input, "#") != NULL)
        {
            //We are dealing with a number address
            strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 4);
            strcat(imm_string, "\0");
            if(atoi(imm_string) <= 4096)
            {
                dec_To_Binary(atoi(imm_string), &imm, 12);
            }
            else
            {
                printf("\nERROR: Jump (JALR) exceeds allowed amount\n");
            }
        }
        else
        {
            //appending a ':' to the end of the search label to find the label later on
            int j = 0;
            strncpy(label_input_instruct,strchr(buffer_instruction_input, ',') + 1, 32);
            while(label_input_instruct[j] != NULL)
            {
                j++;
            }
            char label_modified[j + 1];
            label_modified[j-1] = ':';
            label_modified[j] = '\0';
            strncpy(label_modified, label_input_instruct, j-1);

            //Convert a label to a memory address
            if(return_labels_memory_address(file_length_input, label_modified, labels_input, &labels_mem_address_input, &number_of_labels) <= 4096)
            {
                dec_To_Binary(return_labels_memory_address(file_length_input, label_modified, labels_input, &labels_mem_address_input, &number_of_labels), &imm, 12);
            } 
            else
            {
                printf("\nERROR: Jump from label (JALR) exceeds allowed amount\n");
            }
        }
        for(int i = 0; i < 12; i++)
        {
            buffer_machine[i] = imm[i] +'0';
        } 

        //Input RS1 (BITS [19:15])
        int rs1[5];
        char rs1_string[3];
        strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
        strcat(rs1_string, "\0");
        dec_To_Binary(atoi(rs1_string), &rs1, 5);
        for(int i = 0; i < 5; i++)
        {
            buffer_machine[i + 12] = rs1[i] +'0';
        }

        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';

        //Input opcode (BITS [6:0])
        buffer_machine[25] = '1';
        buffer_machine[26] = '1';
        buffer_machine[27] = '0';
        buffer_machine[28] = '0';
        buffer_machine[29] = '1';
        buffer_machine[30] = '1';
        buffer_machine[31] = '1';      
    }
    else if(strncmp(instruction_header,"JAL", 3) == 0 )
    {
        
        //Input Imm[19:0] (BITS [31:12])
        int imm[20];
        char imm_string[8];
        if(strstr(buffer_instruction_input, "#") != NULL)
        {
            //We are dealing with a number address
            strcpy(imm_string, strstr(buffer_instruction_input, "#") + 1);
            strcat(imm_string, "\0");
            if(atoi(imm_string) <= 1048576)
            {
                dec_To_Binary(atoi(imm_string), &imm, 20);
            }
            else
            {
                printf("\nERROR: Jump (JAL) exceeds allowed amount\n");
            }
        }
        else
        {
            //appending a ':' to the end of the search label to find the label later on
            int j = 0;
            strncpy(label_input_instruct,strchr(buffer_instruction_input, ',') + 1, 32);
            while(label_input_instruct[j] != NULL)
            {
                j++;
            }
            char label_modified[j + 1];
            label_modified[j-1] = ':';
            label_modified[j] = '\0';
            strncpy(label_modified, label_input_instruct, j-1);

            //Convert a label to a memory address
            if(return_labels_memory_address(file_length_input, label_modified, labels_input, labels_mem_address_input, number_of_labels) <= 1048576)
            {
                dec_To_Binary(return_labels_memory_address(file_length_input, label_modified, labels_input, labels_mem_address_input, number_of_labels), &imm, 20); 
            }
            else
            {
                printf("\nERROR: Jump Label (JAL) exceeds allowed amount\n");
            }
        }
        
        buffer_machine[0] = imm[0] +'0';
        for(int i = 0; i < 10; i++)
        {
            buffer_machine[1 + i] = imm[10 + i] +'0';
        }
        buffer_machine[11] = imm[9] +'0';
        for(int i = 0; i < 8; i++)
        {
            buffer_machine[12 + i] = imm[1 + i] +'0';
        }
 
        //Input opcode (BITS [6:0])
        buffer_machine[25] = '1';
        buffer_machine[26] = '1';
        buffer_machine[27] = '0';
        buffer_machine[28] = '1';
        buffer_machine[29] = '1';
        buffer_machine[30] = '1';
        buffer_machine[31] = '1';

    }

    //Input RD (BITS [11:7])
    int rd[5];
    char rd_string[3];
    strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rd_string, "\0");
    dec_To_Binary(atoi(rd_string), &rd, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[20 + i] = rd[i] +'0';
    }

    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine); 

}

//Conditional Branches Instructions Handler
void CBI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6], int file_length_input, char labels_input[file_length_input][32], int* labels_mem_address_input, int* number_of_labels)
{
    //Ex. BEQ x7,x6, #imm <-can also be a label
    //Jump to #imm if x7 == x6
    //Ex. BNQ x7,x6, #imm <-can also be a label
    //Jump to #imm if x7 != x6
    //Ex. BLT x7,x6, #imm <-can also be a label
    //Jump to #imm if x7 < x6
    //Ex. BGE x7,x6, #imm <-can also be a label
    //Jump to #imm if x7 >= x6

    //where x7 = rs1 and x6 = rs2

    char buffer_machine[33];
    char label_input_instruct[32];

    //Input RS1 (BITS [19:15])
    int rs1[5];
    char rs1_string[3];
    strncpy(rs1_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rs1_string, "\0");
    dec_To_Binary(atoi(rs1_string), &rs1, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[12 + i] = rs1[i] +'0';
    }

    //Input RS2 (BITS [24:20])
    int rs2[5];
    char rs2_string[3];
    strncpy(rs2_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs2_string, "\0");
    dec_To_Binary(atoi(rs2_string), &rs2, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[7 + i] = rs2[i] +'0';
    }

    //Input opcode (BITS [6:0])
    buffer_machine[25] = '1';
    buffer_machine[26] = '1';
    buffer_machine[27] = '0';
    buffer_machine[28] = '0';
    buffer_machine[29] = '0';
    buffer_machine[30] = '1';
    buffer_machine[31] = '1';

    //Input Imm[11:0]
    int imm[12];
    char imm_string[5];
    if(strstr(buffer_instruction_input, "#") != NULL)
    {
        //We are dealing with a number address
        strcpy(imm_string, strstr(buffer_instruction_input, "#") + 1);
        strcat(imm_string, "\0");
        if(atoi(imm_string) <= 4096)
        {
            dec_To_Binary(atoi(imm_string), &imm, 12);
        }
        else
        {
            printf("\nERROR: Jump (JAL) exceeds allowed amount\n");
        }
    }
    else
    {
        //appending a ':' to the end of the search label to find the label later on
        int j = 0;
        strncpy(label_input_instruct,strchr(buffer_instruction_input, ',') + 1, 32);
        while(label_input_instruct[j] != NULL)
        {
            j++;
        }
        char label_modified[j + 1];
        label_modified[j-1] = ':';
        label_modified[j] = '\0';
        strncpy(label_modified, label_input_instruct, j-1);

        //Convert a label to a memory address
        if(return_labels_memory_address(file_length_input, label_modified, labels_input, labels_mem_address_input, number_of_labels) <= 4096)
        {
            dec_To_Binary(return_labels_memory_address(file_length_input, label_modified, labels_input, labels_mem_address_input, number_of_labels), &imm, 12); 
        }
        else
        {
            printf("\nERROR: Jump Label (JAL) exceeds allowed amount\n");
        }
    }
    
    buffer_machine[0] = imm[0] +'0';
    for(int i = 0; i < 6; i++)
    {
        buffer_machine[1 + i] = imm[2 + i] +'0';
    }
    for(int i = 0; i < 4; i++)
    {
        buffer_machine[20 + i] = imm[7 + i] +'0';
    }
    buffer_machine[24] = imm[1] +'0';

    if(strncmp(instruction_header,"BEQ", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"BNE", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header,"BLT", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"BGE", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }

    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine); 
}

//Load Instructions Handler
void LI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6])
{
    //examples
    //lw   # rd = mem[rs1+imm]       ; load word
    //lh   # rd = mem[rs1+imm][0:15] ; load half word
    //lhu  # rd = mem[rs1+imm][0:15] ; load half word unsigned
    //lb   # rd = mem[rs1+imm][0:7]  ; load byte
    //lbu  # rd = mem[rs1+imm][0:7]  ; load byte unsigned
    //Example: lw x7,x5,#124 //Loads whatever is at memory(x5 + 124 offset) into x7

    char buffer_machine[33];

    //Input Imm[11:0] (BITS [31:20])
    int imm[12];
    char imm_string[5];
    //We are dealing with a number address
    strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 4);
    strcat(imm_string, "\0");
    if(atoi(imm_string) <= 4096)
    {
        dec_To_Binary(atoi(imm_string), &imm, 12);
    }
    else
    {
        printf("\nERROR: Jump (JALR) exceeds allowed amount\n");
    }

    //Input RS1 (BITS [19:15])
    int rs1[5];
    char rs1_string[3];
    strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs1_string, "\0");
    dec_To_Binary(atoi(rs1_string), &rs1, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[i + 12] = rs1[i] +'0';
    }

    //Input RD (BITS [11:7])
    int rd[5];
    char rd_string[3];
    strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rd_string, "\0");
    dec_To_Binary(atoi(rd_string), &rd, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[20 + i] = rd[i] +'0';
    }

    //Input opcode (BITS [6:0])
    buffer_machine[25] = '0';
    buffer_machine[26] = '0';
    buffer_machine[27] = '0';
    buffer_machine[28] = '0';
    buffer_machine[29] = '0';
    buffer_machine[30] = '1';
    buffer_machine[31] = '1';  

    if(strncmp(instruction_header,"LHU", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header,"LBU", 3) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '1';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"LW", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '1';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"LH", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header,"LB", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }

    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine); 
}

//Store Instructions Handler
void SI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[6])
{
    //examples
    //sw  # mem[rs1+imm] = rs2             ; store word
    //sh  # mem[rs1+imm][0:15] = rs2[0:15] ; store half word
    //sb  # mem[rs1+imm][0:7] = rs2[0:7]   ; store byte
    //Example: sw x7,x5,#124 //store whatever is in register x7 into memorry address( x5 + 124)
    //Template: sw rs2,rs1,#imm_offset
    //x7 = rs2 and x5 = rs1, and #124 = imm

    char buffer_machine[33];

    //Input RS1 (BITS [19:15])
    int rs1[5];
    char rs1_string[3];
    strncpy(rs1_string,strstr(buffer_instruction_input, " ,x") + 2, 2);
    strcat(rs1_string, "\0");
    dec_To_Binary(atoi(rs1_string), &rs1, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[12 + i] = rs1[i] +'0';
    }

    //Input RS2 (BITS [24:20])
    int rs2[5];
    char rs2_string[3];
    strncpy(rs2_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rs2_string, "\0");
    dec_To_Binary(atoi(rs2_string), &rs2, 5);
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[7 + i] = rs2[i] +'0';
    }

    //Input opcode (BITS [6:0])
    buffer_machine[25] = '0';
    buffer_machine[26] = '1';
    buffer_machine[27] = '0';
    buffer_machine[28] = '0';
    buffer_machine[29] = '0';
    buffer_machine[30] = '1';
    buffer_machine[31] = '1'; 

    //Input Imm[11:0]
    int imm[12];
    char imm_string[5];

    //We are dealing with a number address
    strcpy(imm_string, strstr(buffer_instruction_input, "#") + 1);
    strcat(imm_string, "\0");
    if(atoi(imm_string) <= 4096)
    {
        dec_To_Binary(atoi(imm_string), &imm, 12);
    }

    for(int i = 0; i < 7; i++)
    {
        buffer_machine[0 + i] = imm[0 + i] +'0';
    }
    for(int i = 0; i < 5; i++)
    {
        buffer_machine[20 + i] = imm[7 + i] +'0';
    }

    if(strncmp(instruction_header,"SB", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '0';
    }
    else if(strncmp(instruction_header,"SH", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '0';
        buffer_machine[19] = '1';
    }
    else if(strncmp(instruction_header,"SW", 2) == 0)
    {
        //Input funct3 (BITS [14:12])
        buffer_machine[17] = '0';
        buffer_machine[18] = '1';
        buffer_machine[19] = '0';
    }

    //New Line Character (not related)
    buffer_machine[32] = '\0';
    fprintf(MACHINE_File_Input, "%s\n", buffer_machine);
}
