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

typedef struct symbol
{
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
}symbol;

typedef enum {
oddsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym} token_type;

//global variables
int tokenArr[500] = {0};//finalized token array
int tokenIndex = 0;
symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
int tp = 1; //symbol table pointer



void insertSymbolTable(int kind, char name[20], int val, int level, int addr) //insert into symbol table
{
    symbolTable[tp].kind = kind;
    strcpy(symbolTable[tp].name, name);
    symbolTable[tp].val = val;
    symbolTable[tp].level = level;
    symbolTable[tp].addr = val;
    tp++;
}

int symbolTableCheck(char name[10])
{
    for(int i = 0; i < tp; i++)
    {
        if(strcmp(name, symbolTable[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void ConstDeclaration(int** identArr)
{
    int token; //current token
    int tokenIndex = 0; //function's tokenArray index
    token = tokenArr[tokenIndex];

    if (token == constsym)
    {
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: constant keyword must be followed by identifier");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            char tempName [20];
            for(int i = 0; tempName[i] != '#'; i++)
            {
                tempName[i] = identArr[token][i];
            }
            if (symbolTableCheck(tempName) == -1)
            {
                printf("Error: symbol name has already been declared");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != eqsym)
            {
                printf("Error: constants must be assigned with =");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != numbersym)
            {
                printf("Error: constants must be assigned an integer value");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            insertSymbolTable(1, tempName, token, 0, 0);
            tokenIndex++;
            token = tokenArr[tokenIndex];
        } while (token == commasym);
        if (token != semicolonsym)
        {
            printf("Error: constant declarations must be followed by a semicolon");
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
};

int VarDeclaration(int ** identArray, int varCount) //returns number of variables
{
    int numVars = 0;
    int token; //current token
    token = tokenArr[tokenIndex];
    if (token == varsym)
    {
        do
        {
            numVars++;
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: var keywords must be followed by identifier");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            char tempName [20];
            for(int i = 0; tempName[i] != '#'; i++)
            {
                tempName[i] = identArray[token][i];
            }
            if (symbolTableCheck(tempName) != -1)
            {
                printf("Error: symbol name has already been declared");
                exit(0);
            }
            insertSymbolTable(2, tempName, 0, 0, numVars + 2);
            tokenIndex++;
            token = tokenArr[tokenIndex];
        } while (token == commasym);

        if (token != semicolonsym)
        {
            printf("Error: variable declarations must be followed by a semicolon");
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    return(numVars);
};

int STATEMENT(int ** identArray)
{
    int token = tokenArr[tokenIndex]; //current token
    int symIdx;

    if (token == identsym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [10];
        for(int i = 0; tempName[i] != '#'; i++)
        {
            tempName[i] = identArray[token][i];
        }
        symIdx = symbolTableCheck(tempName);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind != 2 /*(not a var)*/)
        {
            printf("Error: only variable values may be altered");
            exit(0);
        }
        if (token != becomessym)
        {
            printf("Error: assignment statements must use :=");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit STO (M = table[symIdx].addr)
        return;
    }
    if (token == beginsym)
    {
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            STATEMENT(identArray);
        }while (token == semicolonsym);
        if (token != endsym)
        {
            printf("Error: begin must be followed by end");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        return;
        
    }
    if (token == ifsym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        CONDITION(identArray);
        // jpcIdx = current code index
        // emit JPC
        if (token != thensym)
        {
            printf("Error: if must be followed by then");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        STATEMENT(identArray);
        // code[jpcIdx].M = current code index
        tokenIndex++;
        token = tokenArr[tokenIndex];
        if(token != fisym)
        {
        //error
        }
        return;
    }
    if (token == whilesym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // loopIdx = current code index
        CONDITION(identArray);
        if (token != dosym)
        {
            printf("Error: while must be followed by do");
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // jpcIdx = current code index
        // emit JPC
        STATEMENT(identArray);
        // emit JMP (M = loopIdx)
        // code[jpcIdx].M = current code index
        return;
    }
    if (token == readsym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        if (token != identsym)
        {
            printf("Error: const, var, and read keywords must be followed by identifier");
        }
        char tempName [10];
        for(int i = 0; tempName[i] != '#'; i++)
        {
            tempName[i] = identArray[token][i];
        }
        symIdx = symbolTableCheck(tempName);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind != 2 /*(not a var)*/)
        {
            printf("Error: only variable values may be altered");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // emit READ
        // emit STO (M = table[symIdx].addr)
        return;
    }
    if (token == writesym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit WRITE
        return;
    }
};
void CONDITION(int ** identArray)
{
    int token = tokenArr[tokenIndex]; //current token
    if (token == oddsym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit ODD
    }
    else
    {
        EXPRESSION(identArray);
        if (token == eqsym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit EQL
        }
        else if (token == neqsym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit NEQ
        }
        else if (token == lessym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LSS
        }
        else if (token == leqsym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LEQ
        }
        else if (token == gtrsym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GTR
        }
        else if (token == geqsym)
        {
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GEQ
        }
        else
        {
            printf("Error: condition must contain comparison operator");
        }
    }
};

void EXPRESSION(int ** identArray)//(HINT: modify it to match the grammar)
{
    int token = tokenArr[tokenIndex]; //current token
    if (token == minussym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        TERM(identArray);
        // emit NEG
        while (token == plussym || token == minussym)
        {
            if (token == plussym)
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
            }
            else
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
            }
        }
    }
    else
    {
        if (token == plussym)
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
        }
        TERM(identArray);
        while (token == plussym || token == minussym)
        {
            if (token == plussym)
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
            }
            else
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
            }
        }
    }
};

void TERM(int ** identArray)
{
    int token = tokenArr[tokenIndex]; //current token
    FACTOR(identArray);
    while (token == multsym || token == slashsym)
    {
        if (token == multsym)
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MUL
        }
        else if (token == slashsym)
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit DIV
        }
        else
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MOD
        }
    }
};

void FACTOR(int ** identArray)
{
    int token = tokenArr[tokenIndex]; //current token
    int symIdx;
    if (token == identsym)
    {
        char tempName [10];
        for(int i = 0; tempName[i] != '#'; i++)
        {
            tempName[i] = identArray[token][i];
        }
        symIdx = symbolTableCheck(tempName);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind == 1) //(const)
        {
            // emit LIT (M = table[symIdx].Value)
        }
        else //(var)
        {
            // emit LOD (M = table[symIdx].addr)
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if (token == numbersym)
    {
        // emit LIT
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if (token == lparentsym)
    {
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        if (token != rparentsym)
        {
            printf("Error: right parenthesis must follow left parenthesis");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else
    {
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");
        exit(0);
    }
};


int main(int argc, char *fileName[])
{
    int fileArr[500] = {0};//array to store input
    int index = 0;//array index
    int curVal = 0;//current input value
    int flag = 0;//EOF flag
    int lastVal;
    int varCount = 0;
    char identArr[50][50] = {'#'};//"#" used for HW3 checking end of strings
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

            // //procedure
            // else if(fileArr[i] == 112 && fileArr[i+1] == 114 && fileArr[i+2] == 111 && fileArr[i+3] == 99 && fileArr[i+4] == 101 && fileArr[i+5] == 100 && fileArr[i+6] == 117 && fileArr[i+7] == 114 && fileArr[i+8] == 101 && (fileArr[i+9] < 65 || fileArr[i+9] > 90) && (fileArr[i+9] < 97 || fileArr[i+9] > 122))
            // {
            //     tokenArr[tokenIndex] = procsym;
            //     tokenIndex++;
            //     i += 9;
            //     printf("procedure 30\n");
            // }

            // //call
            // else if(fileArr[i] == 99 && fileArr[i+1] == 97 && fileArr[i+2] == 108 && fileArr[i+3] == 108 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            // {
            //     tokenArr[tokenIndex] = callsym;
            //     tokenIndex++;
            //     i += 4;
            //     printf("call\t27\n");
            // }

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
            // //else
            // else if(fileArr[i] == 101 && fileArr[i+1] == 108 && fileArr[i+2] == 115 && fileArr[i+3] == 101 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            // {
            //     tokenArr[tokenIndex] = elsesym;
            //     tokenIndex++;
            //     i += 4;
            //     printf("else\t33\n");
            // }
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
                            exit(0);
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
                    exit(0);
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
                exit(0);
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

//end of lexical analyser/scanner_________________________________

//begin of syntactic analyser / parser____________________________
    printf("end scan, begin parser");
    int blockFlag; //0 until program block is done or error

// SYMBOLTABLECHECK (string)
    // linear search through symbol table looking at name
    // return index if found, -1 if not

// PROGRAM
    // BLOCK
    // if token != periodsym
        // error
    // emit HALT

// BLOCK
    while(blockFlag == 0)
    {
        CONSTDECLARATION();
    }
    // numVars = VAR-DECLARATION
    // emit INC (M = 3 + numVars)
    // STATEMENT

// CONST-DECLARATION()
    // if token == const
    // do
        // get next token
        // if token != identsym
            // error
        // if SYMBOLTABLECHECK (token) != -1
            // error
        // save ident name
        // get next token
        // if token != eqlsym
            // error
        // get next token
        // if token != numbersym
            // error
        // add to symbol table (kind 1, saved name, number, 0, 0)
        // get next token
    // while token == commasym
    // if token != semicolonsym
        // error
    // get next token

// VAR-DECLARATION – returns number of variables
    // numVars = 0
    // if token == varsym
        // do
            // numVars++
            // get next token
            // if token != identsym
                // error
            // if SYMBOLTABLECHECK (token) != -1
                // error
            // add to symbol table (kind 2, ident, 0, 0, var# + 2)
            // get next token
        // while token == commasym
        // if token != semicolonsym
            // error
    // get next token
    // return numVars

// STATEMENT
    // if token == identsym
        // symIdx = SYMBOLTABLECHECK (token)
        // if symIdx == -1
            // error
        // if table[symIdx].kind != 2 (not a var)
            // error
        // get next token
        // if token != becomessym
            // error
        // get next token
        // EXPRESSION
        // emit STO (M = table[symIdx].addr)
        // return
    // if token == beginsym
        // do
            // get next token
            // STATEMENT
        // while token == semicolonsym
        // if token != endsym
            // error
        // get next token
        // return
    // if token == ifsym
        // get next token
        // CONDITION
        // jpcIdx = current code index
        // emit JPC
        // if token != thensym
            // error
        // get next token
        // STATEMENT
        // code[jpcIdx].M = current code index
        // return
    // if token == whilesym
        // get next token
        // loopIdx = current code index
        // CONDITION
        // if token != dosym
            // error
        // get next token
        // jpcIdx = current code index
        // emit JPC
        // STATEMENT
        // emit JMP (M = loopIdx)
        // code[jpcIdx].M = current code index
        // return
    // if token == readsym
        // get next token
        // if token != identsym
            // error
        // symIdx = SYMBOLTABLECHECK (token)
        // if symIdx == -1
            // error
        // if table[symIdx].kind != 2 (not a var)
        // error
        // get next token
        // emit READ
        // emit STO (M = table[symIdx].addr)
        // return
    // if token == writesym
        // get next token
        // EXPRESSION
        // emit WRITE
        // return

// CONDITION
    // if token == oddsym
        // get next token
        // EXPRESSION
        // emit ODD
    // else
    // EXPRESSION
        // if token == eqlsym
            // get next token
            // EXPRESSION
            // emit EQL
        // else if token == neqsym
            // get next token
            // EXPRESSION
            // emit NEQ
        // else if token == lessym
            // get next token
            // EXPRESSION
            // emit LSS
        // else if token == leqsym
            // get next token
            // EXPRESSION
            // emit LEQ
        // else if token == gtrsym
            // get next token
            // EXPRESSION
            // emit GTR
        // else if token == geqsym
            // get next token
            // EXPRESSION
            // emit GEQ
        // else
            // error

// EXPRESSION (HINT: modify it to match the grammar)
    // if token == minussym
        // get next token
        // TERM
        // emit NEG
        // while token == plussym || token == minussym
            // if token == plussym
                // get next token
                // TERM
                // emit ADD
            // else
                // get next token
                // TERM
                // emit SUB
    // else
        // if token == plussym
            // get next token
        // TERM
        // while token == plussym || token == minussym
            // if token == plussym
                // get next token
                // TERM
                // emit ADD
            // else
                // get next token
                // TERM
                // emit SUB

// TERM
    // FACTOR
    // while token == multsym || token == slashsym || token == modsym
        // if token == multsym
            // get next token
            // FACTOR
            // emit MUL
        // else if token == slashsym
            // get next token
            // FACTOR
            // emit DIV
        // else
            // get next token
            // FACTOR
            // emit MOD

// FACTOR
    // if token == identsym
        // symIdx = SYMBOLTABLECHECK (token)
        // if symIdx == -1
            // error
        // if table[symIdx].kind == 1 (const)
            // emit LIT (M = table[symIdx].Value)
        // else (var)
            // emit LOD (M = table[symIdx].addr)
        // get next token
    // else if token == numbersym
        // emit LIT
        // get next token
    // else if token == lparentsym
        // get next token
        // EXPRESSION
        // if token != rparentsym
            // error
        // get next token
    // else
        // error

}//main end
