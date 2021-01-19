//
//Created by Tobey Ragain on 12/2/2020
//

/*
 * Program takes a C declaration and decodes it into English
 * Represents how a compiler would decode a declaration
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 64

enum type_tag {IDENTIFIER, QUALIFIER, TYPE};

struct token
{
    unsigned char type;
    char string[MAX_TOKEN_LENGTH];
};

/* Holds tokens we read before reaching first identifier */
struct token stack[MAX_TOKENS];
/* Holds token just read */
struct token this;

int top = -1;

#define pop stack[top--]
#define push(s) stack[++top] = s


enum type_tag classify_string(void)
/*Figure out identifier type */
{
    char * s = this.string;
    if(!strcmp(s, "const"))
    {
        strcpy(s, "read-only");
        return QUALIFIER;
    }

    if(!strcmp(s, "volatile")) return QUALIFIER;
    if(!strcmp(s, "void")) return TYPE;
    if(!strcmp(s, "char")) return TYPE;
    if(!strcmp(s, "signed")) return TYPE;
    if(!strcmp(s, "unsigned")) return TYPE;
    if(!strcmp(s, "short")) return TYPE;
    if(!strcmp(s, "int")) return TYPE;
    if(!strcmp(s, "long")) return TYPE;
    if(!strcmp(s, "float")) return TYPE;
    if(!strcmp(s, "double")) return TYPE;
    if(!strcmp(s, "struct")) return TYPE;
    if(!strcmp(s, "union")) return TYPE;
    if(!strcmp(s, "enum")) return TYPE;
    return IDENTIFIER;
}

void gettoken(void)
/* Reads one char to the stack */
{
    unsigned char * p = (unsigned char *) this.string;

    /* Read past any spaces */
    while((*p = getchar()) == ' ');

    if(isalnum(*p))
    {
        /* It starts with A-Z, 1-9 read in identifier */
        while(isalnum(*++p = getchar()));
        ungetc(*p, stdin);
        *p =            '\0';
        this.type = classify_string();
        return;
    }
    if(*p == '*')
    {
        strcpy(this.string, "pointer to");
        this.type = '*';
        return;
    }
    this.string[1] = '\0';
    this.type = *p;
}

void read_to_first_identifier(void)
/* Reads declaration to the stack */
{
    gettoken();                          /* Gets initial value */
    while(this.type != IDENTIFIER)
    {
        push(this);                      /* Pushes gettoken value into stack */
        gettoken();
    }

    printf("%s is ", this.string);

    gettoken();
}

void deal_with_arrays(void)
{
    while(this.type == '[')
    {
        printf("array ");
        gettoken(); /* A number or ']' */
        if(isdigit(this.string[0]))
        {
            printf("0...%d ", atoi(this.string) - 1);
            gettoken();
            /* Reads the ']' */
        }
        gettoken();
        printf("of ");
    }
}

void deal_with_function_args(void)
{
    while (this.type != ')')
    {
        gettoken();
    }
    gettoken();
    printf("function returning ");

}

void deal_with_pointers(void)
{
    while (stack[top].type == '*')
        printf("%s ", pop.string);
}

void deal_with_declarator(void)
{
    /* Deal with possible arrays/function following identifier */

    switch (this.type)
    {
        case '[': deal_with_arrays(); break;
        case '(':deal_with_function_args();
    }

    deal_with_pointers();

    /* Process tokens that we stacked while reading to identifier */
    while (top >= 0)
    {
        if(stack[top].type == '(')
        {
            pop;
            gettoken(); /* Read past */
            deal_with_declarator();

        }
        else
        {
            printf("%s ", pop.string);
        }
    }
}

int main()
{
   /* Put tokens on stack till it reaches identifier */
   read_to_first_identifier();
   deal_with_declarator();
   printf("\n");
    return 0;
}
