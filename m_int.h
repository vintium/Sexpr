#ifndef M_INT_H_
#define M_INT_H_

typedef struct {
    bool has_value;
    int value;

} m_int;

m_int add_m_int(m_int x, m_int y);
m_int sub_m_int(m_int x, m_int y);
m_int mul_m_int(m_int x, m_int y);
m_int div_m_int(m_int x, m_int y);
int unwrap_m_int(m_int x); 
m_int parse_int(char *x); 

#endif
