
/****************************************************************************

    CPU management
    Copyright (C) 2015 Andre Fachat

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

****************************************************************************/

#include <string.h>

#include "mem.h"
#include "hashmap.h"
#include "astring.h"
#include "position.h"

#include "infiles.h"
#include "cpu.h"
#include "err.h"
#include "cmdline.h"

// corresponds to the classes definitions in af65002.xml 
static const cpu_t cpus[] = {
	{ CPU_NMOS,
		"nmos",			ISA_NMOS_ALL,	
		W_WORD,	false,	false,
		"Standard NMOS 6502, with Decimal mode and illegal opcodes." },
	{ CPU_NMOS_NO_ILLEGAL,
		"nmos_no_illegal",	ISA_BASE + ISA_BCD,	
		W_WORD,	false,	false,
		"Standard NMOS 6502 with decimal mode but without illegal opcodes." },
	{ CPU_CMOS,
		"cmos",			ISA_BASE + ISA_CMOS,	
		W_WORD,	false,	false,
		"Standard 65C02 CMOS, without the Rockwell extensions." },
	{ CPU_RCMOS,
		"cmos_rockwell",	ISA_BASE + ISA_BCD + ISA_CMOS,	
		W_WORD,	false,	false,
		"R65C02 Rockwell CMOS with the RMB/SMB/BBR/BBS extensions." },
	{ CPU_NO_BCD,
		"nmos_no_bcd",		ISA_BASE + ISA_UNDOC,	
		W_WORD,	false,	false,
		"NMOS 6502 with illegal opcodes, but no decimal mode (SED)." },
	{ CPU_NO_BCD_NO_ILLEGAL,
		"nmos_no_illegal_no_bcd", ISA_BASE,	
		W_WORD,	false,	false,
		"NMOS 6502 without illegal opcodes, and no decimal mode." },
	{ CPU_802,
		"65802",		ISA_816_ALL,
		W_816,	true,	false,
		"WDC65802" },
	{ CPU_816,
		"65816",		ISA_816_ALL,	
		W_816,	true,	false,
		"WDC65816" },
	{ CPU_65K,
		"65k",			ISA_65K_ALL,	
		W_QUAD,	false,	true,
		"Generic 65K CPU" },
	{ CPU_65K_W,
		"65k_w",		ISA_65K_ALL,	
		W_WORD,	false,	true,
		"65K CPU in 16 bit width." },
	{ CPU_65K_L,
		"65k_l",		ISA_65K_ALL,	
		W_LONG,	false,	true,
		"65K CPU in 32 bit width." },
	{ CPU_65K_Q,
		"65k_q",		ISA_65K_ALL,	
		W_QUAD,	false,	true,
		"65K CPU in 64 bit width." },
};

static hash_t *cpu_typemap = NULL;

static const char *cpu_key_from_entry(const void *entry) {
	cpu_t *cpu = (cpu_t*) entry;
	return cpu->name;
}

static err_t set_cpu(const char *value, void *extra, int ival) {
        (void) extra;
	(void) ival;

        // TODO
        printf("set cpu: %s\n", value);

        return E_OK;
}

static param_enum_t *cpulist;

static param_enum_t *get_cpus() {
        return cpulist;
}

static cmdline_t cpu_options[] = {
        { "cpu", 	NULL,      PARTYPE_ENUM,   set_cpu,  NULL, NULL,
                "Set the (initial) cpu type", get_cpus },
};

void cpu_module_init() {

	int num_cpus = sizeof(cpus)/sizeof(cpu_t);

	cpulist = cmdline_pval_alloc(num_cpus + 1);

	cpu_typemap = hash_init_stringkey(num_cpus, 5, cpu_key_from_entry);

	int i = 0;
	for (; i < num_cpus; i++) {
		hash_put(cpu_typemap, (void*)&cpus[i]);

		cpulist[i].value = cpus[i].name;
		cpulist[i].description = cpus[i].desc;
	}
	cpulist[i].value = NULL;

	cmdline_register_mult(cpu_options, sizeof(cpu_options)/sizeof(cmdline_t));
}

const cpu_t *cpu_by_name(const char *name) {

	cpu_t *cpu = (cpu_t*) hash_get(cpu_typemap, name);

	return cpu;
}

static inline void error_illegal_cpu_type(const position_t *loc, int requested, int number_of_defined_cpus) {
        loclog_error(loc, "illegal CPU type - requested type %d (of %d)", 
                        requested, number_of_defined_cpus);
}

const cpu_t *cpu_by_type(const position_t *loc, cpu_type type) {

	unsigned int num_cpus = sizeof(cpus)/sizeof(cpu_t);
	if (type >= num_cpus) {
		error_illegal_cpu_type(loc, type, num_cpus);
		return NULL;
	}
	return &cpus[type];
}



