# DXDesigner file format documentation

For those who don't know:
DXDesigner operated on a design blocks concept, meaning your schematic pages grouped into blocks, who are then instanced in the design itselfe.
Each block can consist of multiple pages, and can be instanced multiple times in a design. This makes it easy to reuse repeating schematic pages multiple times in a design.

=
# .prj file
DXDesigner Projects have the file ending .prj, but no actual schematic data is stored inside that file.
The .prj file contains some metadata, and is divided into sections containing different configuration keys
The most important key is 'iCDBDir' in the 'iCDB' section. It defines where the actual schematic data is stored in the 'icdb.dat" file. This key is normally set to '.\database'.
Other important keys might link to the variant information’s file, the bus contents file, the used component library, and the borders configuration file, which are all stored separately.

# icdb.dat
This is the main schematics data file. It is a database, implemented as separate file system.
The files are stored in a file list format, and may consist of multiple data fragments. The data is either zlib compressed, or in plain text.
Please consult the Kaitai file 'icdb.ksy' for a detailed description of the binary database structure.
Using icdbDecode it is possible to extract the data and file structure of a given database.dat file, alongside additional metadata.

# database contents
=
# sids
Session IDs?
This file contains a list of session IDs, and their GUID in the following form:
ID | Version Code inside rectangular parentheses | GUID | either 0 or 1 ? | reason for new ID?

example entry:
1 [3000] abcd-ef01-2345-6789-abcd-ef01-2345-6789-abcd-ef01-2345-6789 0 DxD

#sesog
Session Log?
This file contains a list of windows & DXDesigner version info.
Please consult the Kaitai file 'icdb_seslog.ksy' for a detailed description of the binary data structure.
The number of entries DOES NOT match the number of sids.

#consdef
Constraints Definition?
This file contains a list of parameter.
Please consult the Kaitai file 'icdb_consdef.ksy' for a detailed description of the binary data structure.

# s- folders
Session folders?
For each session (from the sids file) there is a folder named 's' followed by the session index (s1, s2, s3, ...) in the root dir.
The session folders normally contain some of the following elements:
=

# cdbblks folder
(i)CDB Blocks folder?
This folder contains a subfolder for each design block in the design.
The foldername is some longer number (or an address?) with a .blk suffix.
More information on the block folders, consult the *.blk folders chapter.

# cdbcatlg folder
(i)CDB component atlas folder?
TBD

# cdbcmpcache folder
(i)CDB component cache folder?
TBD

# cdbcmps folder
(i)CDB component folder?
TBD

# cdbcnfgs folder
(i)CDB config folder?
TBD

=

# *.blk folders
This folder normaly contains the following files:
dxdatl.v -> (DxD atlas?) Contains information about nets and graphical elements whitin a block page. A parser is currently under development.
blkatl.v -> (Block atlas?) Contains general information about the block (Block Name, used components, border infomation...)