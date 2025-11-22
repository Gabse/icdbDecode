# DXDesigner file format documentation

This project aims at understanding the file format of Siemens EDA (former Mentor Graphics) DXDesigner used in the Pads, Pads Plus and Xpedition design suite.

The research was inspirated by the icdb2fs project done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.

All documentation is purely based on research and observations, no static or dynamic code analysis of proprietary executable files was used.
It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.

European copyright law explicitly allows reverse engineering as a legitime means of obtaining information about a unknown file format,
with the intend of establishing intercompatibility with other software packages.

There is no warranty of any sort on completeness, and correctness of the provided documentation.

# Contents
There are mainly 3 tools in this repo:
+ icdbDecode -> This the main tool. It decompresses database files into a file structure, and attempts to export the schematic data as KiCad files.
+ tools/icdbAnalyzer -> This tool is used to bring the contents of *.v files into a format that is easyla readable, and comparable to other files.
+ tools/icdbCoder -> This tool creates parsing code for a given *.v file used in the icdbDecode application.

A compiled version for windows and linux can be found under the build artifacts, that are updated on every push to the repo.
Windows will throw a warning when you try to run the applications, as they are not signed. You can run them any way, or build them from source yourself.
You can build all applications yourself using CMake and GCC.

There are also the following files in this repo:
+ FileStructure.md -> This file documents the findings about how data is stored in the file structure.
+ DemoFiles.md -> This file contains a list of open accessible schematic files that can be used to test the icdbDecode tool.
+ icdb.ksy -> This file contains a Kaitai listing about how the file structure is stored in the database file
+ icdb_consdef.ksy -> This file contains a Kaitai listing about how data is arranged in the consdef file
+ icdb_seslog.ksy -> This file contains a Kaitai listing about how data is arranged in the seslog file
