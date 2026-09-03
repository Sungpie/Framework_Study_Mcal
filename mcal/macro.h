#ifndef MCAL_MACRO_H

#define MCAL_MACRO_H

/* =========================================================

* 비트 조작 매크로* ========================================================= */

#define MCAL_BIT(pos)           				(1U << (pos))

#define MCAL_SET_BIT(dest, pos)              ((dest) |=  ((unsigned)0x1 << (pos)))

#define MCAL_CLEAR_BIT(dest, pos)            ((dest) &= ~((unsigned)0x1 << (pos)))

#define MCAL_INVERT_BIT(dest, pos)           ((dest) ^=  ((unsigned)0x1 << (pos)))

#define MCAL_CLEAR_AREA(dest, bits, pos)     ((dest) &= ~(((unsigned)(bits)) << (pos)))

#define MCAL_SET_AREA(dest, bits, pos)       ((dest) |=  (((unsigned)(bits)) << (pos)))

#define MCAL_INVERT_AREA(dest, bits, pos)    ((dest) ^=  (((unsigned)(bits)) << (pos)))

#define MCAL_WRITE_BLOCK(dest, bits, data, pos) \ 
((dest) = (((unsigned)(dest) & ~(((unsigned)(bits)) << (pos))) | (((unsigned)(data)) << (pos))))

#define MCAL_EXTRACT_AREA(dest, bits, pos)   ((((unsigned)(dest)) >> (pos)) & (bits))

#define MCAL_CHECK_BIT_SET(dest, pos)        ((((unsigned)(dest)) >> (pos)) & 0x1)

#define MCAL_CHECK_BIT_CLEAR(dest, pos)      (!((((unsigned)(dest)) >> (pos)) & 0x1))

#endif /* MCAL_MACRO_H*/