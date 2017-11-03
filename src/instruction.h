
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


#ifndef OPERATION_H
#define OPERATION_H



typedef enum {
	SY_IMP		= 0,	// implied
	SY_IMM		= 1,	// immediate
	
	SY_ABS		= 2,	// absolute/relative address: zp; word; long; quad; rel; relwide; rellong; relquad
	SY_ABSX		= 3,	// absolute x-indexed: zp,x; word,x; long,x; quad,x
	SY_ABSY		= 4,	// absolute y-indexed: zp,y; word,y; long,y; quad,y
	SY_ABSZ		= 5,	// absolute y-indexed: zp,y; word,y; long,y; quad,y

	SY_IND		= 6,	// zp/word-indirect, word address: (zp); (word)
	SY_INDY		= 7,	// zp/word-indirect, y-indexed, word address: (zp),y; (word),y
	SY_INDZ		= 8,	// zp/word-indirect, y-indexed, word address: (zp),y; (word),y
	SY_XIND		= 9,	// x-indexed, zp/word-indirect, word address: (zp,x); (word,x)

	SY_INDL		= 10,	// zp/word-indirect, long address: [zp]; [word]
	SY_INDYL	= 11,	// zp/word-indirect, y-indexed, long address: [zp],y; [word],y
	SY_INDZL	= 12,	// zp/word-indirect, y-indexed, long address: [zp],y; [word],y
	SY_XINDL	= 13,	// x-indexed, zp/word-indirect, long address: [zp,x]; [word,x]

	SY_INDQ		= 14,	// zp/word-indirect, quad address: [[zp]]; [[word]]
	SY_INDYQ	= 15,	// zp/word-indirect, y-indexed, quad address: [[zp]],y; [[word]],y
	SY_INDZQ	= 16,	// zp/word-indirect, y-indexed, quad address: [[zp]],y; [[word]],y
	SY_XINDQ	= 17,	// x-indexed, zp/word-indirect, quad address: [[zp,x]]; [[word,x]]
	
	SY_MV		= 18,	// two byte 65816 MVN/MVP addressing
	SY_BBREL	= 19,	// zeropage address, plus relative address, R65C02 BBR/BBS
	
	SY_MAX		= 20,	// end

	// virtual addressing modes. Will be mapped to SY_IND, SY_INDL or SY_INDQ depending
	// on memory model used during assembly
	SY_INDD		= 21,	// zp/word-indirect, dynamic address: ((zp)); ((word))
	SY_INDYD	= 22,	// zp/word-zp-indirect, y-indexed, dynamic address: ((zp)),y; ((word)),y
	SY_INDZD	= 23,	// zp/word-zp-indirect, y-indexed, dynamic address: ((zp)),y; ((word)),y
	SY_XINDD	= 24,	// x-indexed, zp/word-indirect, dynamic address: ((zp,x)); ((word,x))

} syntax_type;


typedef enum {
	AM_NONE		= -1,

	AM_IMP		= 0,	// implied
	AM_IMM8		= 1,	// immediate
	AM_IMM16	= 2,	// immediate
	AM_IMM32	= 3,	// immediate
	AM_IMM64	= 4,	// immediate
	
	AM_ABS8		= 5,	// absolute/relative address: zp
	AM_ABS16	= 6,	// absolute/relative address: word
	AM_ABS24	= 7,	// absolute address: word+bank byte
	AM_ABS32	= 8,	// absolute/relative address: long
	AM_ABS64	= 9,	// absolute/relative address: quad

	AM_ABS8X	= 10,	// absolute x-indexed: zp,x
	AM_ABS16X	= 11,	// absolute x-indexed: word,x
	AM_ABS32X	= 12,	// absolute x-indexed: long,x
	AM_ABS64X	= 13,	// absolute x-indexed: quad,x

	AM_ABS8Y	= 14,	// absolute y-indexed: zp,y
	AM_ABS16Y	= 15,	// absolute y-indexed: word,y
	AM_ABS32Y	= 16,	// absolute y-indexed: long,y
	AM_ABS64Y	= 17,	// absolute y-indexed: quad,y

	AM_IND8		= 18,	// zp/word-indirect, word address: (zp)
	AM_IND16	= 19,	// zp/word-indirect, word address: (word)
	AM_IND8Y	= 20,	// zp/word-indirect, y-indexed, word address: (zp),y
	AM_IND16Y	= 21,	// zp/word-indirect, y-indexed, word address: (word),y
	AM_IND8Z	= 22,	// zp/word-indirect, z-indexed, word address: (zp),z
	AM_IND16Z	= 23,	// zp/word-indirect, z-indexed, word address: (word),z
	AM_XIND8	= 24,	// x-indexed, zp/word-indirect, word address: (zp,x)
	AM_XIND16	= 25,	// x-indexed, zp/word-indirect, word address: (word,x)

	AM_IND8L	= 26,	// zp/word-indirect, long address: [zp]
	AM_IND16L	= 27,	// zp/word-indirect, long address: [word]
	AM_IND8YL	= 28,	// zp/word-indirect, y-indexed, long address: [zp],y
	AM_IND16YL	= 29,	// zp/word-indirect, y-indexed, long address: [word],y
	AM_IND8ZL	= 30,	// zp/word-indirect, z-indexed, long address: [zp],z
	AM_IND16ZL	= 31,	// zp/word-indirect, z-indexed, long address: [word],z
	AM_XIND8L	= 32,	// x-indexed, zp/word-indirect, long address: [zp,x]
	AM_XIND16L	= 33,	// x-indexed, zp/word-indirect, long address: [word,x]

	AM_IND8Q	= 34,	// zp/word-indirect, quad address: [[zp]]
	AM_IND16Q	= 35,	// zp/word-indirect, quad address: [[word]]
	AM_IND8YQ	= 36,	// zp/word-indirect, y-indexed, quad address: [[zp]],y
	AM_IND16YQ	= 37,	// zp/word-indirect, y-indexed, quad address: [[word]],y
	AM_IND8ZQ	= 38,	// zp/word-indirect, z-indexed, quad address: [[zp]],z
	AM_IND16ZQ	= 39,	// zp/word-indirect, z-indexed, quad address: [[word]],z
	AM_XIND8Q	= 40,	// x-indexed, zp/word-indirect, quad address: [[zp,x]]
	AM_XIND16Q	= 41,	// x-indexed, zp/word-indirect, quad address: [[word,x]]
	
	AM_MV		= 42,	// two byte 65816 MVN/MVP addressing
	AM_BBREL	= 43,	// zeropage address, plus relative address, R65C02 BBR/BBS
	
	AM_MAX		= 44,	// end
} amode_type;
	
typedef enum {
	PG_BASE		= 0,	// base page, no page prefix needed
	PG_EXT		= 1,	// extended page
	PG_QUICK	= 2,	// quick page
	PG_SYS		= 3,	// system page
} pg_type;

typedef enum {
	RS_NOT_SET	= 0,	// not set
        RS_BYTE         = 1,    // byte width 
        RS_WORD         = 2,    // word (2 byte) width
        RS_LONG         = 3,    // long (4 byte) width
        RS_QUAD         = 4     // quad (8 byte) width
} rs_type;

typedef enum {
	LE_NOT_SET	= 0,	// not set
        LE_E            = 1,    // no extension
        LE_S            = 2,    // sign extensions
        LE_0            = 3,    // extend with 0
        LE_1            = 4     // extend with 1
} le_type;


// ordering by SY_*, then AM_*
#define AMODES_MAX      5

typedef struct {
	const char	*pre;
	const char	*post;
	amode_type	amode[AMODES_MAX];
} syntax_t;

typedef struct {
	//bool_t		is_valid;
	unsigned char	code;
	pg_type 	page_prefix;
	const char 	*amodename;
	isa_map		isa;
} opcode_t;

// operation - equivalent to the mnemonic, like "lda", "adc", "inx", ...
typedef struct instruction_s instruction_t;

struct instruction_s {
	const char	*name;
	isa_map		isa;		// what ISA is it from? Also selects BCD and Illegal opcodes
	bool_t		abs_is_rel;	// is branch with relative addressing?
	bool_t		check_ac_w;	// check AC width mode for 65816, for adr modes with check_width
	bool_t		check_idx_w;	// check Index register width mode for 65816 (with check_width)
	instruction_t	*next;
	opcode_t	opcodes[AM_MAX];
};

typedef struct {
	bool_t		set_am_prefix;
	bool_t		rs_is_width;		// use RS-prefix for width
	pg_type		page_prefix;
	unsigned char	code;
} codepoint_t;

void instruction_module_init();

const instruction_t *instruction_find(const char *name);

bool_t opcode_find(const position_t *loc, const context_t *ctx, const instruction_t *op, syntax_type syntax, int opsize_in_bytes,
	codepoint_t *returned_code);

const syntax_t *op_syn_details(syntax_type syn);

inline static char prefix_um_char(bool_t um) {
	return um ? 'U' : '-';
}

inline static char prefix_nf_char(bool_t nf) {
	return nf ? 'N' : '-';
}

inline static char prefix_rs_char(rs_type p) {
	switch (p) {
	case RS_NOT_SET:
		return '-';
	case RS_BYTE:
		return 'B';
	case RS_WORD:
		return 'W';
	case RS_LONG:
		return 'L';
	case RS_QUAD:
		return 'Q';
	}
	return 'x';
}

inline static char prefix_le_char(rs_type p) {
	switch (p) {
	case LE_NOT_SET:
		return '-';
	case LE_0:
		return '0';
	case LE_1:
		return '1';
	case LE_S:
		return 'S';
	case LE_E:
		return 'E';
	}
	return 'x';
}

#endif

