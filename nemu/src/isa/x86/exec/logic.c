#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  // TODO();
  rtl_and(&t1, &id_dest->val, &id_src->val);
	t0 = 0;
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);
	rtl_update_ZFSF(&t1, id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtl_and(&t1, &id_dest->val, &id_src->val);
	t0 = 0;
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);
	rtl_update_ZFSF(&t1, id_dest->width);
	operand_write(id_dest, &t1);

  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_xor(&t1, &id_dest->val, &id_src->val);
	t0 = 0;

	rtl_set_OF(&t0);
	rtl_set_CF(&t0);
	rtl_update_ZFSF(&t1, id_dest->width);
	operand_write(id_dest, &t1);

  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtl_or(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&t2, id_dest->width);
	t0 = 0;
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  if (id_dest->width == 1) {
		id_dest->val = (int8_t)id_dest->val;
	}
	else if (id_dest->width == 2) {
		id_dest->val = (int16_t)id_dest->val;
	}
	rtl_sar(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  rtl_not(&id_dest->val, &id_dest->val);
	operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}

make_EHelper(rol) {
  rtl_li(&s0,id_dest->val);
  for (int i=0; i<id_src->val; i++) {
    rtl_msb(&s1,&s0,id_dest->width);
    rtl_shli(&s0,&s0,1);
    rtl_add(&s0,&s0,&s1);
  }
  operand_write(id_dest,&s0);

  print_asm_template1(rol);
}