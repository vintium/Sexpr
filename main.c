#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define DEBUG false

/*
Create a REPL to evaluate simple Mathematical S-Expressions.

supported operations: +, -, *, /

for example:

/> (+ 34 35)
69
/> (- 70 1)
69
/> (* 23 3)
69
/> (- (* 24 3) 3)
69
/> (/ 276 (+ 2 2))
69

*/

#include "m_int.h"

typedef enum { PLUS, MINUS, MULTIPLY, DIVIDE } op_type;
typedef enum { EXPR, INT_LITERAL, OP } token_type;


char *OP_TYP_NAMES[] = {"+", "-", "*", "/"};
char *TOK_TYP_NAMES[] = {"expr","int","op"};


typedef struct s_exp {
    bool is_value;
    m_int value;
    op_type op;
    int operands_amt;
    struct s_exp **operands;

} sexp;

typedef struct {
    token_type typ;
    char     *text;
    
} token;

char *fmt_token(token* tok) {
    char *out = malloc(sizeof(char) * 512);
    snprintf(out, 511, "Token {typ: %s, text: '%s'}",
            TOK_TYP_NAMES[tok->typ],
            tok->text);
    return out;
}


char *pop_token(char str[], char delim) {
    char *tok;
    
    /*
    // if the string starts with the delim, trim that off
    if (str[0] == delim) {
        memmove(str, str + 1, strlen(str));
    }
    */
    // return null for empty string     
    if (strcmp(str, "") == 0) {
        return NULL;
    }
 
    for (int i = 0; i < strlen(str); i++) {
        // search thru str until we hit delim
        if (str[i] == delim) {
            // allocate a str long enough to fit the token
            tok = malloc(sizeof(char)*i);
            // copy chars from the string up to (but not including)
            // the delim into the tok
            strncpy(tok, str, i);
            // pop the chars from the string up to (and including) the
            // delim from the original string
            memmove(str, str+ i + 1, strlen(str));
            return tok; 
        }
    }
    // delim wasn't found, so tok should be the whole string
    // allocate a str long enough to fit the original string
    tok = malloc(strlen(str));
    // copy chars from the original string into the tok
    strcpy(tok, str);
    // pop the contents of the string
    memmove(str, str + strlen(str), strlen(str));
    return tok;
}

char *pop_nested_expr(char str[]) {
    char *expr;
    int openp_c = 0;
    int closp_c = 0;
    for (int i = 0; i < strlen(str); i++) {
        // go through the string, counting how many open and close parens
        // we've seen. if the amount is the same, that means that each
        // sub_parenthesised group has been accounted for and we have 
        // encapsulated the whole parenthesised group.

        if (str[i] == '(') { openp_c++; }
        if (str[i] == ')') { closp_c++; }
        if (openp_c == closp_c) {
            // allocate a str long enough to fit the expr
            expr = malloc(sizeof(char)*i+1);
            // copy the chars of the expression from the string into
            // the expr
            strncpy(expr, str, i + 1);
            // pop the chars containing the expr from the original string
            memmove(str, str + i + 1, strlen(str));
            return expr;
        }

    }
    printf("ERROR: Unbalanced parenthesis\n");
    return NULL;
}

int remove_parens(char str[]) {
    if (str[0] !='(' || str[strlen(str) - 1] != ')') {
        printf("ERROR: Unfound parenthesis\n");
        return 1;
    }
    
    memmove(str, str+1, strlen(str));
    str[strlen(str) - 1] = '\0';
    return 0;

}


sexp* parse_sexp(char expr[]) {
    #if DEBUG
    printf("parsing '%s' to sexp\n", expr);
    #endif
    
    sexp* res;
    res = malloc(sizeof(sexp));
    
    int err = remove_parens(expr);
    if (err != 0) {
        return NULL;

    }
    
    // tokenize
    #if DEBUG
    printf("Tokenizing '%s'\n", expr);
    #endif

    // TODO: count how many tokens there will be
    // in order to determine how big toks needs to be
    // to save space
    token *toks[strlen(expr)];
    for (int i = 0; i < strlen(expr); i++) { toks[i] = NULL; } 
    token tok;
    tok.text = "placeholder";
    for (int tokc = 0; tok.text != NULL && strcmp(expr, "") != 0; tokc++) {
        // get next token
        if (expr[0] == '(') {
            tok.typ = EXPR;
            tok.text = pop_nested_expr(expr);
            if (tok.text == NULL) { return NULL; }
        } else {
            tok.text = pop_token(expr, ' ');
            // TODO: consider adding string literals, words, keywords, etc...
            if (parse_int(tok.text).has_value == true) {
                tok.typ = INT_LITERAL;
            } else {
                tok.typ = OP;
            }

        }

        //skip empty tokens (caused by consecutive spaces)
        if (strcmp(tok.text, "") == 0) {
            tokc--; continue; 
            #if DEBUG
            printf("skipped empty token\n");
            #endif    
        }
 
        // only parse 3 tokens, skip the rest

        /*
        if (tokc > 2) {
            printf("TODO: Parsing of more than 3 tokens per expression is not implemented, skipping remaining tokens\n");
            break;
        }
        */

        // add token to toks 
        token* tok_cpy = malloc(sizeof(token));
        memcpy(tok_cpy, &tok, sizeof(token));
        toks[tokc] = tok_cpy;
        #if DEBUG
        printf("toks[%d] = '%s'\n", tokc, toks[tokc]->text);
        #endif
    }
    
    
    int toks_len = (sizeof(toks) / sizeof(token*));
    // toks will have all the tokens and then a bunch of NULL,
    // find the actual amount of tokens there are in toks 
    int toks_amt;
    for (int i = 0; i < toks_len; i++) {
        if (toks[i] == NULL) { toks_amt = i; break; }
        else { continue; }
    }
     

    #if DEBUG > 1
    printf("Parsing tokens...\n");
    for (int i = 0; i < toks_amt; i++) {
        printf("%d: %s\n", i, fmt_token(toks[i]));
    }
    #endif
    // parse
    // TODO:
    // refactor this to supporting sexps with
    // an arbitrary amount of operands.
    // TODO: make op parsing part of normal token parsing.
    // remove op/operand dichotomy and remove op from sexp struct.


    // Parse op
    #if DEBUG
    printf("Parsing op from %s       ... ", fmt_token(toks[0])); 
    #endif
    if (toks[0]->typ != OP) {
        printf("ERROR: Expected sexp to begin with op,\n");
        printf("got {typ: %s, text: '%s'} instead.\n",
                TOK_TYP_NAMES[toks[0]->typ],
                toks[0]->text);
        return NULL;
    }

    if (strcmp(toks[0]->text, "+") == 0) { 
        res->op = PLUS;
    } else if (strcmp(toks[0]->text, "-") == 0) {
        res->op = MINUS;
    } else if (strcmp(toks[0]->text, "*") == 0) {
        res-> op = MULTIPLY;
    } else if (strcmp(toks[0]->text, "/") == 0) {
        res-> op = DIVIDE;
    } else {
        #if DEBUG
        printf("X\n");
        #endif
        printf("ERROR: '%s' is not a valid operation\n", toks[0]->text);
        return NULL;

    }
    #if DEBUG
    printf("Sucessfully parsed op\n");
    #endif

    // Parse operands

    // initialize operands
    int operands_amt = toks_amt - 1;
    res->operands_amt = operands_amt;
    sexp **operands = malloc(sizeof(sexp*) * operands_amt);
    for (int i = 0; i < operands_amt; i++) {
        operands[i] = malloc(sizeof(sexp));
    }

    // go through operands
    for (int i = 1; i < operands_amt + 1; i++) {
        assert(toks[i] != NULL);

        #if DEBUG
        printf("Parsing operand from %s ... ", fmt_token(toks[i]));
        #endif
        if (toks[i]->typ == INT_LITERAL) {
            // the operand is a value-only sexp
            operands[i - 1]->is_value = true;
            operands[i - 1]->value = parse_int(toks[i]->text);
        } else if (toks[i]->typ == EXPR) {
            // if the operand is an expression, parse that expression
            // into an sexp (recursively) and add it to operands
            operands[i - 1] = parse_sexp(toks[i]->text);
            // if parsing the operand fails, pass the error on
            if (operands[i - 1] == NULL) { return NULL; }
        } else if (toks[i]->typ == OP) {
            // operands cannot currently be operations
            // TODO: A type system could be used to allow operands to be 
            // of different types, right now they must be integers or
            // sexps which resolve to integers
            printf("ERROR: Ops are not allowed as operands\n");
            return NULL;
        } else {
            printf("Unreachable\n");
            assert(false);
        }
        #if DEBUG
        printf("Sucessfully parsed operand\n");
        #endif
    }
    /*
    if (parse_int(toks[1]).has_value == true &&
        parse_int(toks[2]).has_value == true) {
        operand_1->is_value = 1;
        operand_1->value = parse_int(toks[1]);
        operand_2->is_value = 1;
        operand_2->value = parse_int(toks[2]);        
    } else if (parse_int(toks[1]).has_value == false) {
        operand_1 = parse_sexp(toks[1]);
        if (operand_1 == NULL) { return NULL; }
        operand_2->is_value = 1;
        operand_2->value = parse_int(toks[2]);
    } else if (parse_int(toks[2]).has_value == false) {
        operand_1->is_value = 1;
        operand_1->value = parse_int(toks[1]);
        operand_2 = parse_sexp(toks[2]);
        if (operand_2 == NULL) { return NULL; }
    }

    */

    res->is_value = false;
    res->operands = operands;
    #if DEBUG
    printf("Successfully parsed sexp\n");
    #endif
    return res;
    
}

m_int* eval_sexp(sexp *s) {
    if (s == NULL) {
        return NULL;
    }    
    
    #if DEBUG
    printf("Evaluating sexp...\n");
    printf("  s->is_value: %d\n", s->is_value);
    printf("  s->op: %d\n", s->op);
    #endif
    m_int *res;
    res = malloc(sizeof(m_int));
    
    // evaluate all of the operands
    m_int *params; 
    params = malloc(sizeof(m_int) * s->operands_amt);
    for (int i = 0; i < s->operands_amt; i++) {
        m_int *r = eval_sexp(s->operands[i]);
        if (r == NULL) { return NULL; }
        params[i] = *r;
    }    

    /*
    if (s->operands_amt > 2) {
        printf("ERROR: '%s' expected 2 operands, recieved %d operands.\n", 
                OP_TYP_NAMES[s->op],
                s->operands_amt
        );
        return NULL;
    }
    */
    
    if (s->is_value == true) {
        #if DEBUG == true
        printf("Evaled value_only sexp with val: %d\n", unwrap_m_int(s->value));
        #endif
        *res = s->value;
        return res;
    
    } else if (s->op == PLUS) {
        *res = add_m_int(params, s->operands_amt);
        return res;
    } else if (s->op == MINUS) {
        *res = sub_m_int(params, s->operands_amt);
        return res;
    } else if (s->op == MULTIPLY) {
        *res = mul_m_int(params, s->operands_amt);
        return res;
    } else if (s->op == DIVIDE) {
        *res = div_m_int(params, s->operands_amt);
        return res;
    } else {
        printf("TODO: Cannot eval sexp because %dth op is not implemented.\n", s->op);
        return NULL;
    }
}

char EXIT_CMD[] = "exit";


int main(void) {
    char input [512];
    while (1) {
        printf("\033[95m\\>\033[0m ");
        fgets(input, 512, stdin);
        
        if (strlen(input) <= 0) {
            printf("ERROR: Empty input\n");
            continue;
        }
        
        if (input[strlen(input) - 1] == '\n') {
            #if DEBUG
            //printf("removed newline from input\n");
            #endif
            input[strlen(input) - 1] = '\0';
        }

        if (strcmp(input, EXIT_CMD) == 0) {
            exit(0);
        }
        #if DEBUG
        printf("input: '%s'\n", input);
        #endif
        m_int* m_res = eval_sexp(parse_sexp(input));
        if (m_res == NULL) { continue; }
        int res = unwrap_m_int(*m_res);
        printf("%d\n", res);
    }
}

