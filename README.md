# a65k
Assembler for 6502 and relatives, esp. the 65k family

This repository contains the (work in progress) a65k assembler and tools suite.

So far planned are support for
- 6502
- 65C02
- R65C02
- 65CE02
- 65816
- 65002 (the new 65k lineage)
and possibly more, using a table-driven approach

Tools will include:
- a65k      - the assembler itself
- a65lint   - a 65x02 code formatter
- a65ld     - a linker

Note: this repository has been broken out of the 65002's af65k repository when it became clear that 
it would make sense and not cost too much effort to support a much wider range of processors.
