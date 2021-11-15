#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "m_int.h"

#define NONE { .has_value = false, .value = 0 }

m_int None = NONE;

m_int add_m_int(m_int* args, int argc) {

    int acc = 0;
    for (int i = 0; i < argc; i++) {
        if (args[i].has_value == false) { return None; }
        acc = acc + args[i].value;
    }
    m_int res = { .has_value = true, .value = acc };
    return res;
       
}

m_int sub_m_int(m_int* args, int argc) {
    
    if (args[0].has_value == false) { return None; }
    int acc = args[0].value;
    for (int i = 1; i < argc; i++) {
        if (args[i].has_value == false) { return None; }
        acc = acc - args[i].value;
    }
    m_int res = { .has_value = true, .value = acc };
    return res;
       
}

m_int mul_m_int(m_int* args, int argc) {

    if (args[0].has_value == false) { return None; }
    int acc = args[0].value;
    for (int i = 1; i < argc; i++) {
        if (args[i].has_value == false) { return None; }
        acc = acc * args[i].value;
    }
    m_int res = { .has_value = true, .value = acc };
    return res;
       
}

m_int div_m_int(m_int* args, int argc) {

    if (args[0].has_value == false) { return None; }
    int acc = args[0].value;
    for (int i = 1; i < argc; i++) {
        if (args[i].has_value == false) { return None; }
        acc = acc / args[i].value;
    }
    m_int res = { .has_value = true, .value = acc };
    return res;
       
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


