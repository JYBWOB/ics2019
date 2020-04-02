#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);


// control.c
make_EHelper(call);
make_EHelper(ret);

// data-mov.c
make_EHelper(push);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);


// arith.c
make_EHelper(sub);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(add);
make_EHelper(cmp);

// system.c
make_EHelper(in);

// logic.c
make_EHelper(xor);
make_EHelper(and);
make_EHelper(or);
make_EHelper(test);
make_EHelper(setcc);