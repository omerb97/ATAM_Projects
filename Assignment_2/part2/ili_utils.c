#include <asm/desc.h>

void my_store_idt(struct desc_ptr *idtr) {
	store_idt(idtr);
}

void my_load_idt(struct desc_ptr *idtr) {
	load_idt(idtr);
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
	gate->offset_low = addr & 0xFFFF;
        gate->offset_middle= (addr >> 16) & 0xFFFF;
	gate->offset_high= addr >> 32;
}

unsigned long my_get_gate_offset(gate_desc *gate) {
	unsigned long low_off = gate->offset_low;
	unsigned long mid_off = gate->offset_middle;
	unsigned long high_off = gate->offset_high;
	
	unsigned long  res = high_off<<16;
	res = res + mid_off;
	res = res<<16;
	res = res + low_off;
	return res;	
}
