

////////////////////////////////////////    MACROS DE BITWISE     ///////////////////////////////////////////

#define BIT(x)                   (1<<(x))                   //NO USAR EN EL CODIGO
#define BIT_ON(prt,num_bit)      ((prt) |= BIT(num_bit))   //encender un bit de un puerto y posicion.
#define BIT_OFF(prt,num_bit)     ((prt) &= ~BIT(num_bit)) //apagar un bit de un puerto y posicion.
#define BIT_TOGGLE(prt,num_bit)  ((prt) ^= BIT(num_bit)) //invierte el valor del bit, realiza XOR.