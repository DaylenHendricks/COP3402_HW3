//Homework 3: PL0 Compiler
//Authors: Daylen Hendricks, Brandon Ramlagan
//COP 3402 Spring 2024
//Date 3/10/2024



//libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static values
#define MAXNUM 5 //max number of digits for a number
#define MAXIDENTIFIER 11 //max length of identifier name
#define MAX_SYMBOL_TABLE_SIZE 500 //max size for symbol table

typedef struct symbol
{
    int kind; // const = 1, var = 2, proc = 3
    char name[11]; // name up to 11 chars
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
int varCount = 0;
int token = 0;//token for parser
int finalToken;//token for period check
char assemblyTable[50][3];//table for assembly code
int assemIndex = 0;

void printAssembly();
void block(char identArray[50][12]);
void insertSymbolTable(int kind, char name[11], int val, int level, int addr); //insert into symbol table
int symbolTableCheck(char name[11]);
void ConstDeclaration(char identArr[50][12]);
int VarDeclaration(char identArray[50][12]); //returns number of variables
int STATEMENT(char identArray[50][12]);
void CONDITION(char identArray[50][12]);
void EXPRESSION(char identArray[50][12]);
void TERM(char identArray[50][12]);
void FACTOR(char identArray[50][12]);


int main(int argc, char *fileName[])
{
    int fileArr[500] = {0};//array to store input
    int index = 0;//array index
    int curVal = 0;//current input value
    int flag = 0;//EOF flag
    int lastVal;
    char identArr[50][12];
    for(int i = 0; i <=50 ; i++)//"#" used for HW3 checking end of strings
    {
        for(int j = 0; j <= 50; j++)
        {
        identArr[i][j] = '#';
        }
    }
    FILE* inputFile = fopen(fileName[1], "r");//file name string for input

    // printing source code
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
        // print out lexeme headers
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
            //     //printf("procedure 30\n");
            // }

            // //call
            // else if(fileArr[i] == 99 && fileArr[i+1] == 97 && fileArr[i+2] == 108 && fileArr[i+3] == 108 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            // {
            //     tokenArr[tokenIndex] = callsym;
            //     tokenIndex++;
            //     i += 4;
            //     //printf("call\t27\n");
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
            //     //printf("else\t33\n");
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
            else if(fileArr[i] == 111 && fileArr[i+1] == 100 && fileArr[i+2] == 100)
            {
                tokenArr[tokenIndex] = oddsym;
                tokenIndex++;
                i += 3;
                printf("odd\t31\n");
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
                    printf("Error: Number too long\n");
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
        varCount = 0;

//end of lexical analyser/scanner_________________________________

//begin of syntactic analyser / parser____________________________
    //printf("\n\nend scan\n begin parser:\n");

// PROGRAM
    int token = tokenArr[0];
    tokenIndex = 0; //function's tokenArray index

    tokenIndex = 0;
    block(identArr);
    printf("Final Token:%d", finalToken);
    if (finalToken != periodsym)
    {
        printf("Error: program must end with period");
        exit(0);
    }
    for(int i = 1; i < tp; i++)
    {
        printf("\nkind: %d", symbolTable[i].kind);
        printf("\nname: %s", symbolTable[i].name);
        printf("\nvalue: %d", symbolTable[i].val);
        printf("\nlevel: %d", symbolTable[i].level);
        printf("\naddress: %d", symbolTable[i].addr);
        symbolTable[i].mark = 1;
        printf("\nmark: %d", symbolTable[i].mark);
        printf("\n");
    }
    exit(0);

    //Symbol Table print
// printf("\n\n\n");
// printf("Kind | Name\t | Value | Level | Address | Mark\n");
// for(int i = 0; i < tp; i++)
// {
//     printf("%d \t%s | %d | 0 | %d | 1", symbolTable[tp].kind, symbolTable[tp].name, symbolTable[tp].val, symbolTable[tp].addr);
// }
}

/*end of main____________________________________________________________________________________________________
_________________________________________________________________________________________________________________
_________________________________________________________________________________________________________________*/
void printAssembly()
{
    printf("Line\tOP\tL\tM");
    int counts =  assemIndex;
    assemIndex = 0;
    for(int i = 0; i <= counts; i++)
    {
        for(int j = 0; j<=2; j++)
        {
            printf("%c%c%c\t0\t0", assemblyTable[0],assemblyTable[1],assemblyTable[2] );
        }
    }
};

void block(char identArr[50][12])
{
    printf("\ncalled block");
    token = tokenArr[tokenIndex];
    ConstDeclaration(identArr);
    int numVars = VarDeclaration(identArr);
    //emit INC (M = 3 + numVars)
        assemblyTable[assemIndex][0] = 'I';
        assemblyTable[assemIndex][1] = 'N';
        assemblyTable[assemIndex][2] = 'C';    
        assemIndex++;  
    STATEMENT(identArr);
};

void insertSymbolTable(int kind, char name[11], int val, int level, int addr) //insert into symbol table
{
    printf("\ncalled insertSymbolTable");
    symbolTable[tp].kind = kind;
    strcpy(symbolTable[tp].name, name);
    symbolTable[tp].val = val;
    symbolTable[tp].level = level;
    symbolTable[tp].addr = addr;
    symbolTable[tp].mark = 0;
    printf("|symbol.name: %s", symbolTable[tp].name);
    tp++;
}

int symbolTableCheck(char name[11])
{
    printf("\ncalled symbolTableCheck");
    printf("|Token:%d", token);
    printf("|TP:%d", tp);
    int errcount = 0;//how many dissimilarities
    int i;
    for(i = 0; i <= tp; i++)
    {
        errcount = 0;
        printf("\n|checking name # %d", i);
        for(int j = 0; j <= (token - 1); j++)
        {
            printf("|checking letter:%c", symbolTable[i].name[j]);
            if(symbolTable[i].name[j] != name[j])
            {
                errcount++;
                printf("||error at name/letter index %d", j);
            }
            if(j == (token - 1))
            {
                if(errcount == 0)
                {
                    printf("|WORD FOUND!\n");
                    return i;
                }
            }
        }

    }
    return -1;
}

void ConstDeclaration(char identArr[50][12])
{
    printf("\ncalled ConstDeclaration");
    printf("|Token:%d", token);
    if (token == constsym)
    {printf("|constsym");
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: constant keyword must be followed by identifier");
                exit(0);
            }
            printf("|token is identsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            printf("|next token:%d", token);
            char tempName [11] = {'#'};
            for(int i = 0; identArr[varCount][i] != '#'; i++)
            {
                printf("|%dletter(s) stored:", (i + 1));
                tempName[i] = identArr[varCount][i];
                printf("%c", identArr[varCount][i]);
            }
            printf("|name stored: %s", tempName);
            varCount++;
            printf("varcount:%d", varCount);
            if (symbolTableCheck(tempName) != -1)
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
            printf("end of loop: %d", token);
        } while (token == commasym);
        if (token != semicolonsym)
        {
            printf("Error: constant declarations must be followed by a semicolon");
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        printf("TokenID:%d",tokenIndex);
    }
    printf("const not detected");
};

int VarDeclaration(char identArray[50][12]) //returns number of variables
{
    printf("\ncalled VarDeclaration");
    printf("|Token:%d", token);
    int numVars = 0;
    if (token == varsym)
    {printf("|varsym");
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
            char tempName [11] = {'#'};
            for(int i = 0; identArray[varCount][i] != '#'; i++)
            {
                printf("|%dletter(s), stored:", (i + 1));
                tempName[i] = identArray[varCount][i];
                printf("%c", tempName[i]);
            }
            varCount++;
            printf("|varcount:%d", varCount);
            if (symbolTableCheck(tempName) != -1)
            {
                printf("Error: symbol name has already been declared");
                exit(0);
            }
            insertSymbolTable(2, tempName, 0, 0, numVars + 2);
            printf("|name stored:%s", tempName);
            tokenIndex++;
            token = tokenArr[tokenIndex];
        } while (token == commasym);

        if (token != semicolonsym)
        {
            printf("Error: variable declarations must be followed by a semicolon");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    return(numVars);
};

int STATEMENT(char identArray[50][12])
{
    printf("\ncalled STATEMENT");
    printf("|Token:%d", token);
    int symIdx;

    if (token == identsym)
    {printf("|identsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [11] = {'#'};
        for(int i = 0; identArray[varCount][i] != '#'; i++)
        {
            printf("|%dletter(s), stored:", (i + 1));
            tempName[i] = identArray[varCount][i];
            printf("%c", tempName[i]);
        }
        printf("|name stored:%s", tempName);
        varCount++;
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
        printf("Token should be 20:%d", token);
        if (token != becomessym)
        {
            printf("|token:%d", token);
            printf("Error: assignment statements must use :=");
            exit(0);
        }
        printf("|becomesym");
        tokenIndex++;
        finalToken = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit STO (M = table[symIdx].addr)
        assemblyTable[assemIndex][0] = 'S';
        assemblyTable[assemIndex][1] = 'T';
        assemblyTable[assemIndex][2] = 'O';    
        assemIndex++;  
        return(0);
    }
    if (token == beginsym)
    {printf("|beginsym");
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            STATEMENT(identArray);
            printf("whiletoken:%d", token);
        }while (token == semicolonsym);
    
        tokenIndex++;
        tokenIndex++;
        token = tokenArr[tokenIndex];
        printf("|token:%d", token);

        if (token != endsym)
        {
            printf("Error: begin must be followed by end");
            exit(0);
        }
        tokenIndex++;
        finalToken = tokenArr[tokenIndex];
        printf("token before return: %d", finalToken);
        return(0);
    }
    if (token == ifsym)
    {printf("|ifsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        CONDITION(identArray);
        // jpcIdx = current code index
        // emit JPC
        assemblyTable[assemIndex][0] = 'J';
        assemblyTable[assemIndex][1] = 'P';
        assemblyTable[assemIndex][2] = 'C';    
        assemIndex++;  
        if (token != thensym)
        {printf("|thensym");
            printf("Error: if must be followed by then");
            exit(0);
        }
        printf("|thensym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        STATEMENT(identArray);
        printf("then statement done");
        printf("token %d", token);
        // code[jpcIdx].M = current code index
        if(token != fisym)
        {
        printf("Error: then must be followed by fi");
        exit(0);
        }
        printf("|fisym");
        return(0);
    }
    if (token == whilesym)
    {printf("|whilesym");
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
        assemblyTable[assemIndex][0] = 'J';
        assemblyTable[assemIndex][1] = 'P';
        assemblyTable[assemIndex][2] = 'C';    
        assemIndex++;  
        STATEMENT(identArray);
        // emit JMP (M = loopIdx)
        assemblyTable[assemIndex][0] = 'J';
        assemblyTable[assemIndex][1] = 'M';
        assemblyTable[assemIndex][2] = 'P';    
        assemIndex++;  
        // code[jpcIdx].M = current code index
        return(0);
    }
    if (token == readsym)
    {printf("|readsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        if (token != identsym)
        {
            printf("Error: const, var, and read keywords must be followed by identifier");
        }
        char tempName [20] = {'#'};
        for(int i = 0; tempName[i] <= token; i++)
        {
            tempName[i] = identArray[varCount][i];
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
        return(0);
    }
    if (token == writesym)
    {printf("|writesym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit WRITE
        return(0);
    }
};

void CONDITION(char identArray[50][12])
{
    printf("\ncalled CONDITION");
    printf("|Token:%d", token);
    if (token == oddsym)
    {printf("|oddsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit ODD
        assemblyTable[assemIndex][0] = 'O';
        assemblyTable[assemIndex][1] = 'D';
        assemblyTable[assemIndex][2] = 'D';    
        assemIndex++;  
    }
    else
    {
        EXPRESSION(identArray);
        if (token == eqsym)
        {printf("|eqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit EQL
        assemblyTable[assemIndex][0] = 'E';
        assemblyTable[assemIndex][1] = 'Q';
        assemblyTable[assemIndex][2] = 'L';    
        assemIndex++;  
        }
        else if (token == neqsym)
        {printf("|neqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit NEQ
        assemblyTable[assemIndex][0] = 'N';
        assemblyTable[assemIndex][1] = 'E';
        assemblyTable[assemIndex][2] = 'Q';    
        assemIndex++;  
        }
        else if (token == lessym)
        {printf("|lessym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LSS
        assemblyTable[assemIndex][0] = 'L';
        assemblyTable[assemIndex][1] = 'S';
        assemblyTable[assemIndex][2] = 'S';    
        assemIndex++;  
        }
        else if (token == leqsym)
        {printf("|leqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LEQ
        assemblyTable[assemIndex][0] = 'L';
        assemblyTable[assemIndex][1] = 'E';
        assemblyTable[assemIndex][2] = 'Q';    
        assemIndex++;  
        }
        else if (token == gtrsym)
        {printf("|gtrsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GTR
        assemblyTable[assemIndex][0] = 'G';
        assemblyTable[assemIndex][1] = 'T';
        assemblyTable[assemIndex][2] = 'R';    
        assemIndex++;  
        }
        else if (token == geqsym)
        {printf("|geqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GEQ
        assemblyTable[assemIndex][0] = 'G';
        assemblyTable[assemIndex][1] = 'E';
        assemblyTable[assemIndex][2] = 'Q';    
        assemIndex++;  
        }
        else
        {
            printf("Error: condition must contain comparison operator");
        }
    }
};

void EXPRESSION(char identArray[50][12])//(HINT: modify it to match the grammar)
{
    printf("\ncalled EXPRESSION");
    printf("|Token:%d", token);
    TERM(identArray);

    if (token == minussym)
    {printf("|minussym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        TERM(identArray);  
        while (token == plussym || token == minussym)
        {
            if (token == plussym)
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
        assemblyTable[assemIndex][0] = 'A';
        assemblyTable[assemIndex][1] = 'D';
        assemblyTable[assemIndex][2] = 'D';    
        assemIndex++;   
            }
            else
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
        assemblyTable[assemIndex][0] = 'S';
        assemblyTable[assemIndex][1] = 'U';
        assemblyTable[assemIndex][2] = 'B';    
        assemIndex++;   
            }
        }
    }
    else if (token == plussym)
    {
        printf("|plussym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        TERM(identArray);
        while (token == plussym || token == minussym)
        {
            if (token == plussym)
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
        assemblyTable[assemIndex][0] = 'A';
        assemblyTable[assemIndex][1] = 'D';
        assemblyTable[assemIndex][2] = 'D';    
        assemIndex++;   
            }
            else
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
        assemblyTable[assemIndex][0] = 'S';
        assemblyTable[assemIndex][1] = 'U';
        assemblyTable[assemIndex][2] = 'B';    
        assemIndex++;  
            }
        }
    }
};

void TERM(char identArray[50][12])
{
    printf("\ncalled TERM");
    token = tokenArr[tokenIndex]; //current token
    printf("|Token:%d", token);

    FACTOR(identArray);
    printf("\nFACTOR->TERM loop");
    printf("|Token:%d", token);
    while (token == multsym || token == slashsym)
    {
        if (token == multsym)
        {printf("|multsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MUL
        assemblyTable[assemIndex][0] = 'M';
        assemblyTable[assemIndex][1] = 'U';
        assemblyTable[assemIndex][2] = 'L';    
        assemIndex++;   
        }
        else if (token == slashsym)
        {printf("|slashsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit DIV
        assemblyTable[assemIndex][0] = 'D';
        assemblyTable[assemIndex][1] = 'I';
        assemblyTable[assemIndex][2] = 'V';    
        assemIndex++;   
        }
        else
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MOD
        assemblyTable[assemIndex][0] = 'M';
        assemblyTable[assemIndex][1] = 'O';
        assemblyTable[assemIndex][2] = 'D';    
        assemIndex++;   
        }
    }
};

void FACTOR(char identArray[50][12])
{
    printf("\ncalled FACTOR");
    printf("|Token:%d", token);
    int symIdx;
    if ((tokenArr[tokenIndex]) == identsym)
    {printf("|identsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [11] = {'#'};
        for(int i = 0; i < token; i++)
        {
            printf("|%dletter(s), stored:", (i + 1));
            tempName[i] = identArray[varCount][i];
            printf("%c", tempName[i]);
        }
        printf("|name stored:%s", tempName);
        varCount++;
        symIdx = symbolTableCheck(tempName);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind == 1) //(const)
        {
            // emit LIT (M = table[symIdx].Value)
        assemblyTable[assemIndex][0] = 'L';
        assemblyTable[assemIndex][1] = 'I';
        assemblyTable[assemIndex][2] = 'T';    
        assemIndex++;
        }
        else //(var)
        {
            // emit LOD (M = table[symIdx].addr)
        assemblyTable[assemIndex][0] = 'L';
        assemblyTable[assemIndex][1] = 'O';
        assemblyTable[assemIndex][2] = 'D';    
        assemIndex++;   
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if ((tokenArr[tokenIndex]) == numbersym)
    {printf("|numbersym");

        assemblyTable[assemIndex][0] = 'L';
        assemblyTable[assemIndex][1] = 'I';
        assemblyTable[assemIndex][2] = 'T';
        assemIndex++;   

        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if (token == lparentsym)
    {printf("|lparentsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        if (token != rparentsym)
        {printf("|rparentsym");
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
