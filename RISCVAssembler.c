#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 64


//MAX NUMBERS OF LINES = 2^31 lines
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
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5]);
void IRRI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5]);
void save_labels_address(int file_length_input, char asm_file_input_name[32], char labels_input[file_length_input][64], int* labels_mem_address_input, int* input_num_labels);
int check_label_exists(char buffer_instruction_input[BUFFER_SIZE]);
int return_labels_memory_address(int file_length_input, char buffer_instruction_input[BUFFER_SIZE], char labels_input[file_length_input][64], int* labels_mem_address_input, int* number_of_labels);

int main(void) {

    char ASM_File_Name[32];
    char Machine_File_Name[48] = "";
    int file_length;
    int current_line = 1;
    int dec_instruct_address = 0;
    char buffer[BUFFER_SIZE];
    char prev_buffer[BUFFER_SIZE];
    char buffer_instruction[5];

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

    
    char labels[file_length][64]; //labels shall not be more than 32 characters in size including ':' and '.' 
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
            strncpy(buffer_instruction, buffer + k, 4);
            strcat(buffer_instruction, "\0");
            
            //IRII (I-Type Instructions)
            if(strcmp(buffer_instruction, "ADDI") == 0 || strcmp(buffer_instruction, "ANDI") == 0 || strcmp(buffer_instruction, "ORI") == 0 ||
               strcmp(buffer_instruction, "XORI") == 0 || strcmp(buffer_instruction, "SLTI") == 0 || strcmp(buffer_instruction, "NOP") == 0 ||
               strcmp(buffer_instruction, "SLLI") == 0 || strcmp(buffer_instruction, "SRLI") == 0 || strcmp(buffer_instruction, "SRAI") == 0 ||
               strcmp(buffer_instruction, "LUI") == 0  || strcmp(buffer_instruction, "AUIPC") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            //IRRI (R-Type Instructions)
            else if(strcmp(buffer_instruction, "ADD") == 0 || strcmp(buffer_instruction, "SLT") == 0 || strcmp(buffer_instruction, "SLTU") == 0 ||
                    strcmp(buffer_instruction, "AND") == 0 || strcmp(buffer_instruction, "OR") == 0 || strcmp(buffer_instruction, "XOR") == 0 ||
                    strcmp(buffer_instruction, "SLL") == 0 || strcmp(buffer_instruction, "SRL") == 0 || strcmp(buffer_instruction, "SUB") == 0 ||
                    strcmp(buffer_instruction, "SRA") == 0)
            {
                IRRI_Handle(buffer, Machine_File, buffer_instruction);
            }
            //Control Transfer Instructions (J-Type Instructions)
            else if(strcmp(buffer_instruction, "JAL") == 0 || strcmp(buffer_instruction, "JALR") == 0)
            {

            }
            //Conditional Branches (B-Type Instructions)
            else if(strcmp(buffer_instruction, "BEQ") == 0 || strcmp(buffer_instruction, "BNE") == 0 ||
                    strcmp(buffer_instruction, "BLT") == 0 || strcmp(buffer_instruction, "BGE") == 0) 
            {

            }
            //Load Instructions (I-Type Instructions)
            else if(strcmp(buffer_instruction, "LW") == 0 || strcmp(buffer_instruction, "LH") == 0 || strcmp(buffer_instruction, "LHU") == 0 ||
                    strcmp(buffer_instruction, "LB") == 0 || strcmp(buffer_instruction, "LBU") == 0)
            {

            }
            //Store Instructions (S-Type Instructions)
            else if(strcmp(buffer_instruction, "SW") == 0 || strcmp(buffer_instruction, "SH") == 0 || strcmp(buffer_instruction, "SB") == 0 )
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
    while(current_dec > 0)
    {
        *(binary_output + (sizeOfArray - 1) - decimal_place) = (current_dec % 2);
        current_dec /= 2;
        decimal_place++;
    }
}

void save_labels_address(int file_length_input, char asm_file_input_name[32], char labels_input[file_length_input][64], int* labels_mem_address_input, int* input_num_labels)
{
    FILE *ASM_File_Input; 
    ASM_File_Input = fopen(asm_file_input_name, "r");
    int current_line = 1;
    char buffer[BUFFER_SIZE];
    char prev_buffer[BUFFER_SIZE];
    int index = 0;
    int mem_address = 0;
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
                int i = 0;
                while(buffer[i] != NULL)
                {
                    i++;
                }
                strcpy(labels_input[0] + 64*index,buffer);
                //strcpy(labels_input[0] + 64*index + (i - 1), "\0");
            }
            else
            {
                strcpy(labels_input[0],buffer);
                labels_mem_address_input[0] = 0;
            }
            //printf("%d\n", labels_mem_address_input[index]);
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
int return_labels_memory_address(int file_length_input, char buffer_instruction_input[BUFFER_SIZE], char labels_input[file_length_input][64], int* labels_mem_address_input, int* number_of_labels)
{
    for(int i = 0; i < *number_of_labels; i++)
    {
        if(strstr(labels_input[i],buffer_instruction_input) != NULL)
        {
            return labels_mem_address_input[i];
        }
    }
    printf("ERROR: Segmentation Fault, Label Does Not Exist\n");
    return -1;
}

//Integer Register-Immediate Instructions Handler
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5])
{
    char buffer_machine[33];
    int startingIndex = 0;
    if(strcmp(instruction_header,"ADDI") == 0 || strcmp(instruction_header,"ANDI") == 0 || strcmp(instruction_header,"ORI") == 0 || strcmp(instruction_header,"XORI") == 0 || 
       strcmp(instruction_header,"SLTI") == 0)
       {
            //Input Imm[11:0] (BITS [31:20])
            int imm[12];
            char imm_string[5];
            strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 4);
            strcat(imm_string, "\0");
            dec_To_Binary(atoi(imm_string), &imm, 12);
            for(int i = 0; i < 12 - 1; i++)
            {
                if(imm[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 12; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[i] = imm[i] +'0';
                }
                else
                {
                    buffer_machine[i] = '0';
                }
            }

            //Input RS1 (BITS [19:15])
            int rs1[5];
            char rs1_string[3];
            strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
            strcat(rs1_string, "\0");
            dec_To_Binary(atoi(rs1_string), &rs1, 5);
            startingIndex = 0;
            for(int i = 0; i < 5 - 1; i++)
            {
                if(rs1[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 5; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[i + 12] = rs1[i] +'0';
                }
                else
                {
                    buffer_machine[i + 12] = '0';
                }
            }

            //Input RD (BITS [11:7])
            int rd[5];
            char rd_string[3];
            strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
            strcat(rd_string, "\0");
            dec_To_Binary(atoi(rd_string), &rd, 5);
            startingIndex = 0;
            for(int i = 0; i < 5 - 1; i++)
            {
                if(rd[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 5; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[20 + i] = rd[i] +'0';
                }
                else
                {
                    buffer_machine[20 + i] = '0';
                }
            }

            if(strcmp(instruction_header,"ADDI") == 0)
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
            else if(strcmp(instruction_header, "ANDI") == 0)
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
            else if(strcmp(instruction_header, "ORI") == 0)
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
            else if(strcmp(instruction_header, "XORI") == 0)
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
            else if(strcmp(instruction_header, "SLTI") == 0)
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
    else if(strcpy(instruction_header, "NOP") == 0)
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
    else if(strcpy(instruction_header, "SLLI") == 0 || strcpy(instruction_header, "SRLI") == 0 || strcpy(instruction_header, "SRAI"))
    {
        //Input Imm[4:0] (BITS [24:20])
        int imm[5];
        char imm_string[3];
        strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 2);
        strcat(imm_string, "\0");
        dec_To_Binary(atoi(imm_string), &imm, 5);
        int startingIndex = 0;
        for(int i = 0; i < 5 - 1; i++)
        {
            if(imm[i] > 1)
            {
                startingIndex = i + 1;
            }
        }
        for(int i = 0; i < 5; i++)
        {
            if(i >= startingIndex)
            {
                buffer_machine[i + 7] = imm[i] +'0';
            }
            else
            {
                buffer_machine[i + 7] = '0';
            }
        }

        //Input RS1 (BITS [19:15])
            int rs1[5];
            char rs1_string[3];
            strncpy(rs1_string,strstr(buffer_instruction_input, ",R") + 2, 2);
            strcat(rs1_string, "\0");
            dec_To_Binary(atoi(rs1_string), &rs1, 5);
            startingIndex = 0;
            for(int i = 0; i < 5 - 1; i++)
            {
                if(rs1[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 5; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[i + 12] = rs1[i] +'0';
                }
                else
                {
                    buffer_machine[i + 12] = '0';
                }
            }

            //Input RD (BITS [11:7])
            int rd[5];
            char rd_string[3];
            strncpy(rd_string,strstr(buffer_instruction_input, " R") + 2, 2);
            strcat(rd_string, "\0");
            dec_To_Binary(atoi(rd_string), &rd, 5);
            startingIndex = 0;
            for(int i = 0; i < 5 - 1; i++)
            {
                if(rd[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 5; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[20 + i] = rd[i] +'0';
                }
                else
                {
                    buffer_machine[20 + i] = '0';
                }
            }

            //imm[11:5] BITS[31:25]
            buffer_machine[0] = 0;
            buffer_machine[1] = 0;
            buffer_machine[2] = 0;
            buffer_machine[3] = 0;
            buffer_machine[4] = 0;
            buffer_machine[5] = 0;
            buffer_machine[6] = 0;
            if(strcmp(instruction_header, "SRAI") == 0)
            {
                buffer_machine[1] = 1;
            }

            //funct3 BITS[14:12] && OPCODE BITS[6:0]
            if(strcmp(instruction_header, "SLLI") == 0)
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
            else if(strcmp(instruction_header, "SRLI") == 0)
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
            else if(strcmp(instruction_header, "SRAI") == 0)
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
    else if(strcpy(instruction_header, "LUI") == 0 || strcpy(instruction_header, "AUIPC") == 0)
    {
        //Ex. LUI R4,#imm
        //Ex. AUIPC R5,#imm

        //Input Imm[19:0] (BITS [31:12])
            int imm[20];
            char imm_string[8];
            strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 7);
            strcat(imm_string, "\0");
            dec_To_Binary(atoi(imm_string), &imm, 12);
            int startingIndex = 0;
            for(int i = 0; i < 20 - 1; i++)
            {
                if(imm[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 20; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[i] = imm[i] +'0';
                }
                else
                {
                    buffer_machine[i] = '0';
                }
            }
        //Input RD (BITS [11:7])
            int rd[5];
            char rd_string[3];
            strncpy(rd_string,strstr(buffer_instruction_input, " R") + 2, 2);
            strcat(rd_string, "\0");
            dec_To_Binary(atoi(rd_string), &rd, 5);
            startingIndex = 0;
            for(int i = 0; i < 5 - 1; i++)
            {
                if(rd[i] > 1)
                {
                    startingIndex = i + 1;
                }
            }
            for(int i = 0; i < 5; i++)
            {
                if(i >= startingIndex)
                {
                    buffer_machine[20 + i] = rd[i] +'0';
                }
                else
                {
                    buffer_machine[20 + i] = '0';
                }
            }

            if(strcmp(instruction_header, "LUI") == 0)
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
            else if(strcmp(instruction_header, "AUIPC") == 0)
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
void IRRI_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5])
{
    char buffer_machine[33];
    int startingIndex = 0;

    //Input RS2 (BITS [24:20])
    int rs2[5];
    char rs2_string[3];
    strncpy(rs2_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs2_string, "\0");
    dec_To_Binary(atoi(rs2_string), &rs2, 5);
    for(int i = 0; i < 5 - 1; i++)
    {
        if(rs2[i] > 1)
        {
            startingIndex = i + 1;
        }
    }
    for(int i = 0; i < 5; i++)
    {
        if(i >= startingIndex)
        {
            buffer_machine[i + 7] = rs2[i] +'0';
        }
        else
        {
            buffer_machine[i + 7] = '0';
        }
    }

    //Input RS1 (BITS [19:15])
    int rs1[5];
    char rs1_string[3];
    strncpy(rs1_string,strstr(buffer_instruction_input, ",x") + 2, 2);
    strcat(rs1_string, "\0");
    dec_To_Binary(atoi(rs1_string), &rs1, 5);
    for(int i = 0; i < 5 - 1; i++)
    {
        if(rs1[i] > 1)
        {
            startingIndex = i + 1;
        }
    }
    for(int i = 0; i < 5; i++)
    {
        if(i >= startingIndex)
        {
            buffer_machine[i + 12] = rs1[i] +'0';
        }
        else
        {
            buffer_machine[i + 12] = '0';
        }
    }

    //Input RD (BITS [11:7])
    int rd[5];
    char rd_string[3];
    strncpy(rd_string,strstr(buffer_instruction_input, " x") + 2, 2);
    strcat(rd_string, "\0");
    dec_To_Binary(atoi(rd_string), &rd, 5);
    startingIndex = 0;
    for(int i = 0; i < 5 - 1; i++)
    {
        if(rd[i] > 1)
        {
            startingIndex = i + 1;
        }
    }
    for(int i = 0; i < 5; i++)
    {
        if(i >= startingIndex)
        {
            buffer_machine[20 + i] = rd[i] +'0';
        }
        else
        {
            buffer_machine[20 + i] = '0';
        }
    }

     //Input opcode (BITS [6:0])
    buffer_machine[25] = '0';
    buffer_machine[26] = '1';
    buffer_machine[27] = '1';
    buffer_machine[28] = '0';
    buffer_machine[29] = '0';
    buffer_machine[30] = '1';
    buffer_machine[31] = '1';

    if(strcmp(instruction_header, "ADD") == 0)
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
    else if(strcmp(instruction_header, "SUB") == 0)
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
    else if(strcmp(instruction_header, "SLL") == 0)
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
    else if(strcmp(instruction_header, "SLT") == 0)
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
    else if(strcmp(instruction_header, "SLTU") == 0)
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
    else if(strcmp(instruction_header,"XOR") == 0)
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
    else if(strcmp(instruction_header, "SRL") == 0)
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
    else if(strcmp(instruction_header, "SRA") == 0)
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
    else if(strcmp(instruction_header, "OR") == 0)
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
    else if(strcmp(instruction_header, "AND") == 0)
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
void CTI_Handle()
{
    //Ex. JAL Rd,#imm
    //Ex. JALR
    //return_labels_memory_address(file_length, buffer, labels, &labels_mem_address, &number_of_labels);
}

//Conditional Branches Instructions Handler
void CBI_Handle()
{

}

//Load Instructions Handler
void LI_Handle()
{

}

//Store Instructions Handler
void SI_Handle()
{

}
