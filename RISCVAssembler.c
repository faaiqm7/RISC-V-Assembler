#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 65536

/*RISC-V Instructions*/
char IRII[36][5] = {"ADDI", "ANDI","OIR","XORI","SLTI","NOP",
                    "SLLI","SRLI","SRAI","LUI","AUPIC","ADD",
                    "SLT","SLTU","AND","OR","XOR","SLL",
                    "SRL","SUB","SRA","JAL","JALR","BEQ",
                    "BNE","BLT","BGE","LW","LH","LHU",
                    "LB","LBU","SW","SH","SB"};

//Headers
int file_number_lines(char asm_file_input_name[32]);
void dec_To_Binary(int dec_input, int* binary_output, int sizeOfArray);
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5]);

int main(void) {

    char ASM_File_Name[32];
    char Machine_File_Name[48] = "";
    int file_length;
    int current_line = 1;
    char buffer[BUFFER_SIZE];
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

    if(file_length > 0)
    {
        ASM_File = fopen(ASM_File_Name, "r");
        Machine_File = fopen(Machine_File_Name, "w");
        while(current_line <= file_length)
        {   
            //Detect instruction
            fgets(buffer, BUFFER_SIZE, ASM_File);
            strncpy(buffer_instruction, buffer, 4);
            strcat(buffer_instruction, "\0");
            
            if(strcmp(buffer_instruction, "ADDI") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            else if(strcmp(buffer_instruction, "ANDI") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            else if(strcmp(buffer_instruction, "OIR") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            else if(strcmp(buffer_instruction, "XORI") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            else if(strcmp(buffer_instruction, "SLTI") == 0)
            {
                IRII_Handle(buffer, Machine_File, buffer_instruction);
            }
            else if(strcmp(buffer_instruction, "NOP") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SLLI") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SRLI") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SRAI") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LUI") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "AUPIC") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "ADD") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SLT") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SLTU") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "AND") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "OR") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "XOR") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SLL") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SRL") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SUB") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SRA") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "JAL") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "JALR") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "BEQ") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "BNE") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "BLT") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "BGE") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LW") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LH") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LHU") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LB") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "LBU") == 0)
            {
                
            }
            
            else if(strcmp(buffer_instruction, "SW") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SH") == 0)
            {
                
            }
            else if(strcmp(buffer_instruction, "SB") == 0)
            {
                
            }
            else
            {
                printf("ERROR: Unrecognized instruction at LINE %d\n", current_line);
                break;
            }
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

//Integer Register-Immediate Instructions Handler
void IRII_Handle(char buffer_instruction_input[BUFFER_SIZE], FILE* MACHINE_File_Input, char instruction_header[5])
{
    char buffer_machine[33];
    if(strcmp(instruction_header,"ADDI") == 0 || strcmp(instruction_header,"ANDI") == 0 || strcmp(instruction_header,"OIR") == 0 || strcmp(instruction_header,"XORI") == 0 || 
       strcmp(instruction_header,"SLTI") == 0)
       {

            //Input Imm[11:0] (BITS [31:20])
            int imm[12];
            char imm_string[5];
            strncpy(imm_string,strchr(buffer_instruction_input, '#') + 1, 4);
            strcat(imm_string, "\0");
            dec_To_Binary(atoi(imm_string), &imm, 12);
            int startingIndex = 0;
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
                buffer_machine[27] = '0';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '0';
                buffer_machine[31] = '1';

            }
            else if(strcmp(instruction_header, "ANDI") == 0)
            {
                //Ex. ANDI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    ANDI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '0';
                buffer_machine[18] = '0';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '0';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '0';
            }
            else if(strcmp(instruction_header, "OIR") == 0)
            {
                //Ex. OIR Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    OIR R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '0';
                buffer_machine[18] = '1';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
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
                buffer_machine[17] = '0';
                buffer_machine[18] = '1';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '0';
                buffer_machine[29] = '1';
                buffer_machine[30] = '0';
                buffer_machine[31] = '0';
            }
            else if(strcmp(instruction_header, "SLTI") == 0)
            {
                //Ex. SLTI Rd,Rs1,#imm (ONLY FOR THIS FORMAT)
                //    SLTI R25,R12,#6

                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '0';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '0';
                buffer_machine[29] = '1';
                buffer_machine[30] = '0';
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
        buffer_machine[27] = '0';
        buffer_machine[28] = '0';
        buffer_machine[29] = '0';
        buffer_machine[30] = '0';
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
                buffer_machine[17] = '1';
                buffer_machine[18] = '0';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '1';
                buffer_machine[29] = '0';
                buffer_machine[30] = '0';
                buffer_machine[31] = '0';
            }
            else if(strcmp(instruction_header, "SRLI") == 0)
            {
                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '1';
                buffer_machine[19] = '0';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '1';
                buffer_machine[29] = '0';
                buffer_machine[30] = '0';
                buffer_machine[31] = '1';
            }
            else if(strcmp(instruction_header, "SRAI") == 0)
            {
                //Input funct3 (BITS [14:12])
                buffer_machine[17] = '1';
                buffer_machine[18] = '1';
                buffer_machine[19] = '1';

                //Input opcode (BITS [6:0])
                buffer_machine[25] = '0';
                buffer_machine[26] = '0';
                buffer_machine[27] = '0';
                buffer_machine[28] = '1';
                buffer_machine[29] = '0';
                buffer_machine[30] = '1';
                buffer_machine[31] = '0';
            }
    }
    else if(strcpy(instruction_header, "LUI") == 0 || strcpy(instruction_header, "AUPIC") == 0)
    {
        //Ex. LUI R4,#imm
        //Ex. AUPIC R5,#imm
    }

    //New Line Character (not related)
    buffer_machine[32] = '\n';
    fputs(buffer_machine, MACHINE_File_Input);
    
}

void ADD_Handle()
{

}

void SLT_Handle()
{

}

void SLTU_Handle()
{

}

void AND_Handle()
{

}

void OR_Handle()
{

}

void XOR_Handle()
{

}

void SLL_Handle()
{

}

void SRL_Handle()
{

}

void SUB_Handle()
{

}

void SRA_Handle()
{

}

void JAL_Handle()
{

}

void JALR_Handle()
{

}

void BEQ_Handle()
{

}

void BNE_Handle()
{

}

void BLT_Handle()
{

}

void BGE_Handle()
{

}

void LW_Handle()
{

}

void LH_Handle()
{

}

void LHU_Handle()
{

}

void LB_Handle()
{

}

void LBU_Handle()
{

}

void SW_Handle()
{

}

void SH_Handle()
{

}

void SB_Handle()
{

}