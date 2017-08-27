WRN:>>> label adc #123
WRN:BLK:0x1c0e720 lab:0x1c0e7c0 op:0x613b18 par:0x1c0e800
WRN:LAB:0x1c0e7c0 name:label ctx:0x1c0cdc0 pos:bogusfile:1
WRN:OPR:0x613b18 name:ADC isa:$561 isrel:0 acw:0 idxw:0
WRN:SYN: 1
WRN:PAR: 
WRN:    A:len=1
WRN:      type=v, modifier=0 ( ), op=0( ), val=(d)123/7b
WRN:>>> lda 123
WRN:BLK:0x1c222a0 lab:(nil) op:0x611dd8 par:0x1c22340
WRN:OPR:0x611dd8 name:LDA isa:$1561 isrel:0 acw:0 idxw:0
WRN:SYN: 2
WRN:PAR: 
WRN:    A:len=1
WRN:      type=v, modifier=0 ( ), op=0( ), val=(d)123/7b
WRN:>>> ldx (12),y
WRN:BLK:0x1c35de0 lab:(nil) op:0x611a30 par:0x1c36000
WRN:OPR:0x611a30 name:LDX isa:$401 isrel:0 acw:0 idxw:0
WRN:SYN: 7
WRN:PAR: 
WRN:    A:len=1
WRN:      type=v, modifier=0 ( ), op=0( ), val=(d)12/c
WRN:>>> lda ($12,s,x)
WRN:BLK:0x1c49aa0 lab:(nil) op:0x611dd8 par:0x1c49cc0
WRN:OPR:0x611dd8 name:LDA isa:$1561 isrel:0 acw:0 idxw:0
WRN:BASE: 151
WRN:SYN: 9
WRN:PAR: 
WRN:    A:len=1
WRN:      type=v, modifier=0 ( ), op=0( ), val=(h)18/12
WRN:>>> lda (12,s),y
WRN:BLK:0x1c5d760 lab:(nil) op:0x611dd8 par:0x1c5d980
WRN:OPR:0x611dd8 name:LDA isa:$1561 isrel:0 acw:0 idxw:0
WRN:BASE: 151
WRN:SYN: 7
WRN:PAR: 
WRN:    A:len=1
WRN:      type=v, modifier=0 ( ), op=0( ), val=(d)12/c
