//Homework 3: PL0 Compiler
//Authors: Daylen Hendricks, Brandon Ramlagan
//COP 3402 Spring 2024
//Date 3/8/2024

//libraries
#include <stdio.h>
#include <stdlib.h>

//static values
#define MAXNUM 5 //max number of digits for a number
#define MAXIDENTIFIER 11 //max length of identifier name
#define MAX_SYMBOL_TABLE_SIZE 500 //max size for symbol table

//global variables
int tokenArr[500] = {0};//finalized token array
int tokenIndex = 0;

typedef struct symbol
{
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
}symbolTable;

typedef enum {
skipsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym} token_type;


int main(int argc, char *fileName[])
{
    int fileArr[500] = {0};//array to store input
    int index = 0;//array index
    int curVal = 0;//current input value
    int flag = 0;//EOF flag
    int lastVal;
    int varCount = 0;
    char identArr[50][50];
    FILE* inputFile = fopen(fileName[1], "r");//file name string for input

//printing source code
        printf("Source Program:\n");
        int printFlag = 0;
        for(int i = 0; printFlag == 0; i++)
        {
            curVal = fgetc(inputFile);//getting next value & cast to int
            if(curVal == -1)
            {
                printFlag = 1;
            }
            else
            {
                printf("%c", curVal);
            }
        }

        fseek(inputFile, 0, SEEK_SET);//set file pointer to beginning again

        while(flag == 0)//loop through text and insert in array
        {
            curVal = (int)fgetc(inputFile);//getting next value & cast to int
            if(curVal == 47)//finding "/"
            {
                lastVal = curVal;
                curVal = (int)fgetc(inputFile);//getting next value & cast to int
                if(curVal == 42 && lastVal == 47)//finding "/*"
                {
                    int commentFlag = 0;//flag for comment exclusion loop
                    while(commentFlag == 0)
                    {
                        lastVal = curVal;
                        curVal = (int)fgetc(inputFile);//getting next value & cast to int
                        if(curVal == 47 && lastVal == 42)
                        {
                            break;//want to break out of comment loop
                        }
                    }
                }
                else//if just "/"
                {
                fileArr[index] = lastVal;//store "/"
                index++;//move on
                }
            }
            else if(curVal == -1)//end of file detection
            {
                flag = 1;
            }
            else if(curVal >= 0 && curVal <= 32)// \n to whitespace
            {
                fileArr[index] = 32;//insert value into array
                index++;//incrementing to next array index
            }
            else
            {
                fileArr[index] = curVal;//insert value into array
                index++;//incrementing to next array index
            }
        }
        //print out lexeme headers
        printf("\nLexeme Table:\n\n");
        printf("lexeme token type\n");


//begin tokenization
        int i = 0; 
        int flag1 = 0;    
        while(flag1 == 0)
        {

    //reserved words        
            if(fileArr[i] == 32)//space
            {
                i++;
            }
            //const
            else if(fileArr[i] == 99 && fileArr[i+1] == 111 && fileArr[i+2] == 110 && fileArr[i+3] == 115 && fileArr[i+4] == 116 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = constsym;
                tokenIndex++;
                i += 5;
                printf("const\t28\n");
            }
            
            //var
            else if(fileArr[i] == 118 && fileArr[i+1] == 97 && fileArr[i+2] == 114 && (fileArr[i+3] < 65 || fileArr[i+3] > 90) && (fileArr[i+3] < 97 || fileArr[i+3] > 122))
            {
                tokenArr[tokenIndex] = varsym;
                tokenIndex++;
                i += 3;
                printf("var\t29\n");
            }

            //procedure
            else if(fileArr[i] == 112 && fileArr[i+1] == 114 && fileArr[i+2] == 111 && fileArr[i+3] == 99 && fileArr[i+4] == 101 && fileArr[i+5] == 100 && fileArr[i+6] == 117 && fileArr[i+7] == 114 && fileArr[i+8] == 101 && (fileArr[i+9] < 65 || fileArr[i+9] > 90) && (fileArr[i+9] < 97 || fileArr[i+9] > 122))
            {
                tokenArr[tokenIndex] = procsym;
                tokenIndex++;
                i += 9;
                printf("procedure 30\n");
            }

            //call
            else if(fileArr[i] == 99 && fileArr[i+1] == 97 && fileArr[i+2] == 108 && fileArr[i+3] == 108 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = callsym;
                tokenIndex++;
                i += 4;
                printf("call\t27\n");
            }

            //begin
            else if(fileArr[i] == 98 && fileArr[i+1] == 101 && fileArr[i+2] == 103 && fileArr[i+3] == 105 && fileArr[i+4] == 110 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = beginsym;
                tokenIndex++;
                i += 5;
                printf("begin\t21\n");
            }

            //end
            else if(fileArr[i] == 101 && fileArr[i+1] == 110 && fileArr[i+2] == 100 && (fileArr[i+3] < 65 || fileArr[i+3] > 90) && (fileArr[i+3] < 97 || fileArr[i+3] > 122))
            {
                tokenArr[tokenIndex] = endsym;
                tokenIndex++;
                i += 3;
                printf("end\t22\n");
            }
            //if
            else if(fileArr[i] == 105 && fileArr[i+1] == 102 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = ifsym;
                tokenIndex++;
                i += 2;
                printf("if\t23\n");
            }
            //fi?
            else if(fileArr[i] == 102 && fileArr[i+1] == 105 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = fisym;
                tokenIndex++;
                i += 2;
                printf("fi\t8\n");
            }
            //then
            else if(fileArr[i] == 116 && fileArr[i+1] == 104 && fileArr[i+2] == 101 && fileArr[i+3] == 110 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = thensym;
                tokenIndex++;
                i += 4;
                printf("then\t24\n");
            }
            //else
            else if(fileArr[i] == 101 && fileArr[i+1] == 108 && fileArr[i+2] == 115 && fileArr[i+3] == 101 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = elsesym;
                tokenIndex++;
                i += 4;
                printf("else\t33\n");
            }
            //while
            else if(fileArr[i] == 119 && fileArr[i+1] == 104 && fileArr[i+2] == 105 && fileArr[i+3] == 108 && fileArr[i+4] == 101 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = whilesym;
                tokenIndex++;
                i += 5;
                printf("while\t25\n");
            }
            //do
            else if(fileArr[i] == 100 && fileArr[i+1] == 111 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = dosym;
                tokenIndex++;
                i += 2;
                printf("do\t26\n");
            }
            //read
            else if(fileArr[i] == 114 && fileArr[i+1] == 101 && fileArr[i+2] == 97 && fileArr[i+3] == 100 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = readsym;
                tokenIndex++;
                i += 4;
                printf("read\t32\n");
            }
            //write
            else if(fileArr[i] == 119 && fileArr[i+1] == 114 && fileArr[i+2] == 105 && fileArr[i+3] == 116 && fileArr[i+4] == 101 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = writesym;
                tokenIndex++;
                i += 5;
                printf("write\t31\n");
            }
            


    //identifiers
            else if((fileArr[i] >= 65 && fileArr[i] <= 90) || (fileArr[i] >= 97 && fileArr[i] <= 122))
            {
                int flag2 = 0;
                int identLen = 0;
                tokenArr[tokenIndex] = identsym;
                tokenIndex++;
                while(flag2 == 0)
                {
                    if((fileArr[i] >= 65 && fileArr[i] <= 90) || (fileArr[i] >= 97 && fileArr[i] <= 122) || (fileArr[i] >= 48 && fileArr[i] <= 57))
                    {
                                identArr[varCount][identLen] = fileArr[i];
                                printf("%c", fileArr[i]);
                                i++;
                                identLen++;
                    }
                     else
                    {
                        flag2 = 1;
                        if(identLen > 11)
                        {
                            printf("\tName too long\n");
                        }
                        else
                        {
                            printf("\t2\n");

                        }
                    }
                }
                tokenArr[tokenIndex] = identLen;
                tokenIndex++;
                varCount++;



            }
//numbers
            else if(fileArr[i] >= 48 && fileArr[i] <= 57)
            {
                char num[20];
                tokenArr[tokenIndex] = numbersym;
                tokenIndex++;
                int numLen = 0;
                while(fileArr[i] >= 48 && fileArr[i] <= 57)
                {
                    num[numLen] = fileArr[i];
                    numLen++;
                    i++;
                }
                int c = atoi(num);
                printf("%d", c);
                tokenArr[tokenIndex] = c;
                tokenIndex++;
                if(numLen >5)
                {
                    printf("\tNumber too long\n");
                }
                else
                {
                    printf("\t3\n");
                }

            }
//symbols
            //lparentsym
           else if(fileArr[i] == 40)
            {
                tokenArr[tokenIndex] = lparentsym;
                tokenIndex++;
                i++;
                printf("(\t15\n");
            }
            //rparentsym
           else if(fileArr[i] == 41)
            {
                tokenArr[tokenIndex] = rparentsym;
                tokenIndex++;
                i++;
                printf(")\t16\n");
            }
            //mult
           else if(fileArr[i] == 42)
            {
                tokenArr[tokenIndex] = multsym;
                tokenIndex++;
                i++;
                printf("*\t6\n");
            }
            //plus
           else if(fileArr[i] == 43)
            {
                tokenArr[tokenIndex] = plussym;
                tokenIndex++;
                i++;
                printf("+\t4\n");
            }
            //comma
           else if(fileArr[i] == 44)
            {
                tokenArr[tokenIndex] = commasym;
                tokenIndex++;
                i++;
                printf(",\t17\n");
            }
            //minus
           else if(fileArr[i] == 45)
            {
                tokenArr[tokenIndex] = minussym;
                tokenIndex++;
                i++;
                printf("-\t5\n");
            }
            //period
            else if(fileArr[i] == 46)
            {
                tokenArr[tokenIndex] = periodsym;
                tokenIndex++;
                i++;
                flag1 = 1;
                printf(".\t19\n\n\n");
            }
            //slash
           else if(fileArr[i] == 47)
            {
                tokenArr[tokenIndex] = slashsym;
                tokenIndex++;
                i++;
                printf("/\t7\n");
            }
            //become
            else if(fileArr[i] == 58 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = becomessym;
                tokenIndex++;
                i += 2;
                printf(":=\t20\n");
            }
            //semicolon
            else if(fileArr[i] == 59)
            {
                tokenArr[tokenIndex] = semicolonsym;
                tokenIndex++;
                i++;
                printf(";\t18\n");
            }
            //equal
            else if(fileArr[i] == 61)
            {
                tokenArr[tokenIndex] = eqsym;
                tokenIndex++;
                i++;
                printf("=\t9\n");
            }
            //not equal
            else if(fileArr[i] == 60 && fileArr[i+1] == 62)
            {
                tokenArr[tokenIndex] = neqsym;
                tokenIndex++;
                i += 2;
                printf("<>\t10\n");
            }
          //less than or equal
            else if(fileArr[i] == 60 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = leqsym;
                tokenIndex++;
                i += 2;
                printf("<=\t12\n");
            }
          //less than
            else if(fileArr[i] == 60)
            {
                tokenArr[tokenIndex] = lessym;
                tokenIndex++;
                i++;
                printf("<\t11\n");
            }
            //greater or equal
            else if(fileArr[i] == 62 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = geqsym;
                tokenIndex++;
                i+=2;
                printf(">=\t14\n");
            }
            //greater than
            else if(fileArr[i] == 62)
            {
                tokenArr[tokenIndex] = gtrsym;
                tokenIndex++;
                i++;
                printf(">\t13\n");
            }
            else
            {
                printf("%c\tInvalid Symbol\n", fileArr[i]);
                i++;
            }
        }//tokenize while loop end

        //printing token list
        printf("Token List:\n");
        varCount = 0;
        for(int i = 0; i < tokenIndex; i++)
        {
            if(tokenArr[i] == identsym)
            {
                printf("%d ", tokenArr[i]);
                i++;
                for(int j = 0; j < tokenArr[i]; j++)
                {
                    printf("%c", identArr[varCount][j]);
                }
                printf(" ");
                varCount++;
                i++;
            }
            printf("%d ", tokenArr[i]);
        }
}//main end
