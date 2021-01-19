//
// Created by Tobey Ragain on 12/16/20.
//

/*Implements a finite state machine for decoding C decorations */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum
{
    Idle_state,
    dec_read_state,
    pointer_dealt_with_state,
    func_arg_dealt_with_state,
    array_dealt_with_state,
    dealt_with_inner_func_state,
    last_state
} eSystemState;

typedef enum
{
    read_to_first_identifier_event,
    deal_with_pointer_event,
    deal_with_func_arg_event,
    deal_with_array_event,
    deal_with_inner_func_event,
    last_event
} eSystemEvent;

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

int top = 0;

//Stack actions
#define pop stack[top--]
#define push(s) stack[++top] = s

//typedef for function pointer for event handler function
typedef eSystemState (*fpEventHandler)(void);

typedef struct
{
    eSystemState eStateMachine;
    eSystemEvent eStateMachineEvent;
    fpEventHandler sStateMachineHandler;
} sStateMachine;


enum type_tag classify_string(void)
/*Figure out identifier type */
{
    char * s = this.string;
    if(!strcmp(s, "const"))
    {
        strcpy(s, "that is read-only");
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

eSystemState deal_with_arrays_handler(void)
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
    return array_dealt_with_state;
}

//Deals with function
eSystemState deal_with_func_args_handler(void)
{
    if(this.type == '(')
    {
        while (this.type != ')')
        {
            gettoken();
        }
        gettoken();
        printf("function returning ");
    }

    return func_arg_dealt_with_state;
}

eSystemState deal_with_pointer_handler(void)
{
    while (stack[top].type == '*')
        printf("%s ", pop.string);
    return pointer_dealt_with_state;
}

eSystemState deal_with_inner_func_handler(void)
{
    //Checks if current token is a starting parenthesis
    if(stack[top].type == '(')
    {
        pop;
        gettoken(); /* Read past */
        return dec_read_state;
    }
    else
    {
        printf("%s ", pop.string);          //Pops a token from stack and prints what is on it
        return dealt_with_inner_func_state;
    }
}

eSystemState read_to_first_identifier_handler(void)
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
    return dec_read_state;
}

//Initialize array of structure with states and event with proper handler
sStateMachine asStateMachine [] =
        {
                {Idle_state, read_to_first_identifier_event, read_to_first_identifier_handler},
                {dec_read_state, deal_with_pointer_event, deal_with_pointer_handler},
                {pointer_dealt_with_state, deal_with_func_arg_event, deal_with_func_args_handler},
                {func_arg_dealt_with_state, deal_with_array_event, deal_with_arrays_handler},
                {array_dealt_with_state, deal_with_inner_func_event, deal_with_inner_func_handler}
        };

int main(void)
{
    eSystemState eNextState = Idle_state;

    //Runtime for FSM
    do
    {
        eSystemEvent eNewEvent = asStateMachine[eNextState].eStateMachineEvent;

        //Checks to see if end of enum list has been reached
        if((eNextState < last_state) && (eNewEvent < last_event) && (asStateMachine[eNextState].eStateMachineEvent == eNewEvent) && (asStateMachine[eNextState].sStateMachineHandler != NULL))
        {
            eNextState = (*asStateMachine[eNextState].sStateMachineHandler)();
        }
        else if(eNextState == dealt_with_inner_func_state)      //Resets FSM for another loop if necessary
        {
            eNextState = dec_read_state;                        //Resets to top of state enum

        }
    }
    while(top > 0);                                             //Continued if tokens are on stack

}













