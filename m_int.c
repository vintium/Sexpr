#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "m_int.h"

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


