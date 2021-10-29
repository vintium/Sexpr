#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

typedef struct maybe_int {
    bool has_value;
    int value;

} m_int;

m_int add_m_int(m_int x, m_int y) {
    if (x.has_value == true && y.has_value == true) {
        m_int res;
        res.has_value = true;
        res.value = x.value + y.value;
        return res;
    } else {
         m_int none;
        none.has_value = false;
        return none;
    }
}

m_int sub_m_int(m_int x, m_int y) {
    if (x.has_value == true && y.has_value == true) {
        m_int res;
        res.has_value = true;
        res.value = x.value - y.value;
        return res;

    } else {
        m_int none;
        none.has_value = false;
        return none;
    }

}


m_int mul_m_int(m_int x, m_int y) {
    if (x.has_value == true && y.has_value == true) {
        m_int res;
        res.has_value = true;
        res.value = x.value * y.value;
        return res;

    } else {
        m_int none;
        none.has_value = false;
        return none;
    }

}


m_int div_m_int(m_int x, m_int y) { 
    m_int none;
    none.has_value = false;
    if (x.has_value == true && y.has_value == true) {
        if (y.value == 0) {
            return none;
        }
        m_int res;
        res.has_value = true;
        res.value = x.value / y.value;
        return res;
    } else {
        return none;
    }

}

int unwrap_m_int(m_int x) {
    if (x.has_value == true) {
        return x.value;
    }
    else {
        printf("ERROR: Tried to extract nonexistent value from m_int\n");
        exit(1);
    }

}

m_int parse_int(char *x) {
    if (atoi(x) == 0 && strcmp(x, "0") != 0) {
        m_int fail;
        fail.has_value = false;
        return fail;
    }

    m_int res;
    res.has_value = true;
    res.value = atoi(x);
    return res;
}

typedef enum op_types { PLUS, MINUS, MULTIPLY, DIVIDE } op_type;

typedef struct s_exp {
    bool is_value;
    m_int value;
    op_type op;
    struct s_exp *operand_1;
    struct s_exp *operand_2;
} sexp;


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

        // TODO: Error handling here. This currently cannot cope with
        // unmatched parentheses.
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
    return NULL;
}

void remove_parens(char str[]) {
    if (str[0] !='(' || str[strlen(str) - 1] != ')') {
        printf("ERROR: Unfound parenthesis");
        exit(1);
    }
    
    memmove(str, str+1, strlen(str));
    str[strlen(str) - 1] = '\0';


}


sexp* parse_sexp(char expr[]) {
    //printf("parsing '%s' to sexp\n", expr);
    
    
    sexp* res;
    res = malloc(sizeof(sexp));
    
    remove_parens(expr);
    
    // tokenize
    //printf("Tokenizing '%s'\n", expr);
    char *toks[3]; 
    char *tok = "";
    for (int tokc = 0; tok != NULL && strcmp(expr, "") != 0; tokc++) {
        // get next token
        if (expr[0] == '(') {
            tok = pop_nested_expr(expr);
        } else {
            tok = pop_token(expr, ' ');
        }        
        //printf("tok = '%s'\n", tok);
        //skip empty tokens (caused by consecutive spaces)
        if (strcmp(tok, "") == 0) { tokc--; continue; }
        
        // only parse 3 tokens, skip the rest
        if (tokc > 2) {
            printf("TODO: Parsing of more than 3 tokens per expression is not implemented, skipping remaining tokens\n");
            break;
        }
 
        // add token to toks 
        toks[tokc] = malloc(strlen(tok));
        strcpy(toks[tokc], tok);
        //printf("toks[%d] = '%s'\n", tokc, toks[tokc]);

    }
    
     
    // initialize operands
    sexp* operand_1 = malloc(sizeof(sexp));
    sexp* operand_2 = malloc(sizeof(sexp));
    
    //printf("Parsing tokens...\n");
    //printf("0: '%s'\n", toks[0]);
    //printf("1: '%s'\n", toks[1]);
    //printf("2: '%s'\n", toks[2]);
     
    // parse
    // TODO: Refactor this bullshit! it is terrible.
    
    // determine operation
    if (strcmp(toks[0], "+") == 0) { 
        res->op = PLUS;

    } else if (strcmp(toks[0], "-") == 0) {
        res->op = MINUS;
    } else if (strcmp(toks[0], "*") == 0) {
        res-> op = MULTIPLY;
    } else if (strcmp(toks[0], "/") == 0) {
        res-> op = DIVIDE;
    } else {
        printf("ERROR: '%s' is not a valid operation\n", toks[0]);
        exit(1);

    }

    if (parse_int(toks[1]).has_value == true &&
        parse_int(toks[2]).has_value == true) {
        operand_1->is_value = 1;
        operand_1->value = parse_int(toks[1]);
        operand_2->is_value = 1;
        operand_2->value = parse_int(toks[2]);        
    } else if (parse_int(toks[1]).has_value == false) {
        operand_1 = parse_sexp(toks[1]);
        operand_2->is_value = 1;
        operand_2->value = parse_int(toks[2]);
    } else if (parse_int(toks[2]).has_value == false) {
        operand_1->is_value = 1;
        operand_1->value = parse_int(toks[1]);
        operand_2 = parse_sexp(toks[2]);
    }

    res->is_value = false;
    res->operand_1 = operand_1;
    res->operand_2 = operand_2;

    
    return res;
    
}

m_int eval_sexp(sexp *s) {
    //printf("Evaluating sexp...\n");
    //printf("  s->is_value: %d\n", s->is_value);
    //printf("  s->op: %d\n", s->op);
    //printf("  PLUS: %d\n", PLUS);

    if (s->is_value == true) {
        //printf("Evaled value_only sexp with val: %d\n", unwrap_m_int(s->value));
        return s->value;
    } else if (s->op == PLUS) {
        return add_m_int(eval_sexp(s->operand_1), eval_sexp(s->operand_2));
    } else if (s->op == MINUS) {
        return sub_m_int(eval_sexp(s->operand_1), eval_sexp(s->operand_2));
    } else if (s->op == MULTIPLY) {
        return mul_m_int(eval_sexp(s->operand_1), eval_sexp(s->operand_2));
    } else if (s->op == DIVIDE) {
        return div_m_int(eval_sexp(s->operand_1), eval_sexp(s->operand_2));
    } else {
        printf("ERROR: Cannot eval sexp: %d is not implemented.\n", s->op);
        exit(1);
    }
}

char EXIT_CMD[] = "exit";


int main(void) {
    char input [512];
    while (1) {
        printf("\\> ");
        fgets(input, 512, stdin);
        if (strlen(input) > 0 && input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }
        if (strcmp(input, EXIT_CMD) == 0) {
            exit(0);
        }
        //printf("input: '%s'\n", input);
        int res = unwrap_m_int(eval_sexp(parse_sexp(input)));
        printf("%d\n", res);
    }
}

