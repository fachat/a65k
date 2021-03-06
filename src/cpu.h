
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


#ifndef CPU_H
#define CPU_H

typedef enum {
        W_NONE                  = 0,
        W_8                     = 1,
        W_16                    = 2,
        W_24                    = 4,
        W_32                    = 8,
        W_64                    = 16,
} width_map;

// possible register widths (i.e. W_24 3-byte addresses are missing)
#define W_ALLREG        (W_8 + W_16 + W_32 + W_64)
#define W_816           (W_8 + W_16 + W_24)
#define W_BYTE          (W_8)
#define W_WORD          (W_8 + W_16)
#define W_LONG          (W_8 + W_16 + W_32)
#define W_QUAD          (W_8 + W_16 + W_32 + W_64)


// note: the ISA Map corresponds to the feature name in the af65002.xml definition file
typedef enum {
        ISA_BASE                = 1,     // standard NMOS without SED
        ISA_UNDOC	        = 2,     // NMOS "undocumented" opcodes
        ISA_BCD	 	        = 4,     // NMOS SED opcode
        ISA_ROR	 	        = 8,     // NMOS ROR opcode
        ISA_CMOS                = 16,    // CMOS extensions
        ISA_CMOS_IND            = 32,    // CMOS zeropage indirect addressing mode ops
        ISA_CMOS_INDZ           = 64,    // CMOS zeropage indirect addressing mode ops indexed with Z, for ce02
        ISA_RCMOS	        = 128,   // RMB/SMB/BBS/BBR opcodes
        ISA_816                 = 256,   // 65816 extensions to CMOS 6502
        ISA_816CE02             = 512,   // 65816 extensions to CMOS 6502
        ISA_65K                 = 1024,  // 65K extensions to CMOS 6502
        ISA_65K10               = 2048,  // 65K10 system extensions 65k
        ISA_CE02                = 4096,  // 65ce02 extensions to the Rockwell CMOS
} isa_map;

// may at some time correspond to the feature combinations from the class definitions in af65002.xml definition 
#define ISA_NMOS_ALL   	(ISA_BASE + ISA_UNDOC + ISA_BCD)
#define ISA_CMOS_ALL    (ISA_BASE + ISA_CMOS + ISA_RCMOS)
#define ISA_816_ALL     (ISA_BASE + ISA_CMOS + ISA_816)
#define ISA_65K_ALL     (ISA_BASE + ISA_CMOS + ISA_65K)
#define ISA_ALL         (ISA_BASE + ISA_UNDOC + ISA_BCD + ISA_CMOS + ISA_RCMOS + ISA_816 + ISA_65K)


typedef enum {
	CPU_NMOS		= 0,	// legal NMOS opcodes, decimal mode and illegal opcodes
	CPU_NMOS_NO_ILLEGAL	= 1,	// legal NMOS opcodes incl. decimal mode, but no illegal opcodes
	CPU_CMOS		= 2,	// CMOS opcodes (without Rockwell extensions)
	CPU_RCMOS		= 3,	// CMOS plus Rockwell extensions
	CPU_NO_BCD		= 4,	// NMOS with illegal opcodes, but without BCD mode (e.g. NES)
	CPU_NO_BCD_NO_ILLEGAL	= 5,	// NMOS without illegal opcodes, and without BCD mode
	CPU_802			= 6,	// 65802
	CPU_816			= 7,	// 65816
	CPU_65K			= 8,	// base 65k CPU (implicitely 64 bit, modifyable with .width) 
	CPU_65K_W		= 9,	// 65k CPU 16-bit width
	CPU_65K_L		= 10,	// 65k CPU 32-bit width
	CPU_65K_Q		= 11,	// 65k CPU 64-bit width
} cpu_type;

typedef struct {
	const cpu_type	type;
	const char 	*name;
	const isa_map	isa;
	const width_map	width;		// memory model size 16, 24, 32, 64
	//const bool_t	has_bcd;
	//const bool_t	has_illegal;
	const bool_t	has_check_width;
	const bool_t	has_prefixes;
	const char 	*desc;		// description of CPU model
} cpu_t;


void cpu_module_init();

const cpu_t *cpu_by_name(const char *name);

const cpu_t *cpu_by_type(const position_t *loc, cpu_type type);


#endif

