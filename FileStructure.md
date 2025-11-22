# DXDesigner / xDXDesigner file format documentation

For those who don't know:
DX Designer / xDXDesigner operates on a design blocks concept, meaning your schematic pages grouped into blocks (called schematic in DXD), who are then instanced in the design itself.
Each block can consist of multiple pages (called sheets in DXD and groups in the file format), and can be instanced multiple times in a design. This makes it easy to reuse repeating schematic pages multiple times in a design.

# .prj file
DX Designer Projects have the file ending .prj, but no actual schematic data is stored inside that file.
The .prj file contains some metadata, and is divided into sections containing different configuration keys
The most important key is 'iCDBDir' in the 'iCDB' section. It defines where the actual schematic data is stored in the 'icdb.dat" file. This key is normally set to '.\database'.
Other important keys might link to the variant information’s file, the bus contents file, the used component library, and the borders configuration file, which are all stored separately.
It looks something like this:
```
SECTION iCDB
KEY iCDBDir ".\database"
ENDSECTION
```

# icdb.dat
This is the main schematics data file. It is a database, implemented as separate file system.
The files are stored in a file list format, and may consist of multiple data fragments. The data is either zlib compressed, or in plain text.
Please consult the Kaitai file 'icdb.ksy' for a detailed description of the binary database structure.
Using icdbDecode it is possible to extract the data and file structure of a given database.dat file, alongside additional metadata.

# database contents

# .v files
Most of the files inside a database have a .v ending, and contain a list of keys with their respective data payload. The data format is as follows:
- Length of key as 32bit (unsigned) integer.
- Key as string (char array) without zero termination
- Type-code as 32bit (unsigned) integer. The following type-codes are known:
	* `1`	-> String
	* `2`	-> Integer Array
	* `3`	-> UID (2 x 32bit / 64Bit)
	* `4`	-> SUID (4 x 32bit / 128Bit)
	* `5`	-> Int (1 x 32bit)
	* `6`	-> Long (2 x 32bit / 64Bit)
	* `7`	-> Time (3 x 32bit / 96Bit)
	* `8`	-> LongLong (4 x 32bit / 128Bit)
	* `13`	-> GUID (Format unknown, as there was never data present in any of my observations)

- Number of entry’s as (unsigned) integer.
	+ For all type codes except 1(string) and 2(int array) the number must be divided by 8 to obtain the number of entries.
	+ For type-code 1(string) the number is calculated the following way: Constant per entry plus sum of every string length (12 + number of characters rounded up to multiple of 4 per entry)
	+ For type-code 2(int array) the number is calculated the following way: Constant per entry plus sum of every int array length (8 + number of bytes per entry)
- Padding? (16 bytes, or 4 (unsigned) integers of value 0)
- Block Address? ((unsigned) integer number. Not sure what it is intended for)
- Payload Data.
	+ For all type codes except 1(string) and 2(int array) the data is located here, there are however some magic values with special functions:
		* `0x4FFFFFFF` -> This magic marks the end of the data section. It is followed by the length of the next key, or by a 0 for the last entry in a file.
		* `0x4FFFFFFE` -> Unknown function, but always followed wit another integer vaule that does not belong to the data payload.
						Probably change to another block address, with the value of the second value being the new address
		* `0x4FFFFFFD` -> Repeated value. This magic is always followed by the number of repetitions represented as unsigned integer value.
						The data from before the magic is always repeated n times. As such, this magic can probably not exist as first entry in the payload section.
						The number of repetitions is a negative value that is incremented until 0.
		* `0x4FFFFFFC` -> Increasing value. This magic is similar to the repeated value
						Instead of just repeating the value before the magic, the value is incremented by one in respect to the prior entry.
						It is unclear if and how this magic is present on type-codes other than 5(Int).
	+ For type-code 1(string) the first byte of every entry consists of a 8 bit string length value, followed by the string as a char array without zero termination.
		The string length value can also be one of the following magic keys:
		* `0xFF`	-> End of payload, the same as key `0x4FFFFFFF` in other type codes.
		* `0xFE`	-> Unknown function, probably the same as key `0x4FFFFFFE` in other type codes.
		* `0xFD`	-> Marks strings with more than 255 characters. This Key is followed by a (unsigned) integer as string length.
	+ For type-code 2(int array) the first entry of every array consists of (unsigned) integer length value, followed by the integer array payload.
	  Instead of the length, the same magic keys as for other types can be found, marking the data section end etc.
	  It is unclear if the magics for repeated and increasing value can also be present, and what they do.

# sids file
Session IDs?
This file contains a list of session IDs, and their GUID in the following form:
ID | Version Code inside rectangular parentheses | GUID | either 0 or 1 ? | reason for new ID?

example entry:
```
1 [3000] abcd-ef01-2345-6789-abcd-ef01-2345-6789-abcd-ef01-2345-6789 0 DxD
```

# sesog file
Session Log?
This file contains a list of windows & DXDesigner version info.
Please consult the Kaitai file 'icdb_seslog.ksy' for a detailed description of the binary data structure.
The number of entries DOES NOT match the number of sids.

# consdef file
Constraints Definition?
This file contains a list of parameter.
Please consult the Kaitai file 'icdb_consdef.ksy' for a detailed description of the binary data structure.

# s- folders
Session folders?
For each session (from the sids file) there is a folder named 's' followed by the session index (s1, s2, s3, ...) in the root directory.
The session folders normally contain some of the following elements:

## cdbblks folder
(i)CDB Blocks folder?
This folder contains a sub-folder for each design block in the design.
The folder name is the block UID with a .blk suffix.
More information on the block folders, consult the *.blk folders chapter.


### *.blk folders
This folder normally contains the following files:
- dxdatl.v -> (DxD atlas?) Contains information about nets and graphical elements within a block page. A parser is currently under development.
- blkatl.v -> (Block atlas?) Contains general information about the block (Block Name, used components, border information...)

## cdbcatlg folder
(i)CDB component atlas folder?
This folder normally contains the following files:
- catlgatl.v -> (catalog atlas?) Contains general information’s about the project such as schematic pages, their UID and their names.
- grpatl.v -> (Group atlas?) Exact purpose currently unknown. Contains mainly timestamps.
- grpobj -> (Group object?) Contains information about what UIDs belong to group. See section "grpobj" for more information

### grpobj file
This file lists UIDs the corresponding group codes. It is used to assign the design elements to different sheets.
The format is the following:
0x0 - 0x1fff -> Unknown
after 0x2000:
	-> empty int (32 bit)
	-> Number of UID structures to follow as int (32 bit)
	list of UID structures:
		-> UID
		-> number of group codes
		-> list of group codes

## cdbcmpcache folder
(i)CDB component cache folder?
This folder contains a copy of all components used in the schematic.
It normally contains the following files:
- catlgatl.v -> (catalog atlas?) Contains general information’s about the project such as schematic pages, their UID and their names.
- grpatl.v -> (Group atlas?) Exact purpose currently unknown. Contains mainly timestamps.

## cdbcmps folder
(i)CDB component folder?
TBD

## cdbcnfgs folder
(i)CDB config folder?
TBD


# Elements currently known:
This is a list of elements and the keys that hold the data that define them. Some element index into another element. Here it is noted, that indexes to other elements start with 1 not 0 as first entry.

## Schematics
Schematics are a type of nodes that are used to build a hierarchy. They can one or multiple pages of schematic diagrams, so-called sheets.
 - MdlNam in catlgatl.v (Type String) => Name of the Block
 - BlkUID in catlgatl.v (Type UID) => Unique identifier of the block. Used to index into .blk folder where the block data is stored.
 
## Gropups
Sub modules of blocks so to speak. Groups are indexed from sheets. 
 - GroupDescription in catlgatl.v (Type String) => Empty string most of the time
 - GroupDxDView in catlgatl.v (Type Int) => Not sure what this does. It does sometimes have a different number of entry’s than the other sheet keys
 - GroupName in catlgatl.v (Type String) => Name of the Sheet
 - GroupOwnerUID in catlgatl.v (Type UID) => Not sure what this does.
 - GroupType in catlgatl.v (Type Int) => Not sure what this does. 2 most of the time.
 - GroupUID in catlgatl.v (Type UID) => UID of the group.
 
## Sheets (called blocks in the file format)
Sheets are what hold the schematic diagram elements. There can be multiple sheets in a Schematic.
- Block2Grp in dxdatl.v (Type int) => Index into the group elements, defining the name and other attributes of the block. This code is also used with the grpobj file to check if a element with a certain UID is in this block or not.
- BlockBBox in dxdatl.v (Type Long) => Custom Paper size (X(Int), Y(Int)) in 10nm. Also defines the page orientation.
- BlockSize in dxdatl.v (Type int) => Fixed Paper size codes:
	+ 0 = A
	+ 1 = B
	+ 2 = C
	+ 3 = D
	+ 4 = E
	+ 5 = A4
	+ 6 = A3
	+ 7 = A2
	+ 8 = A1
	+ 9 = A0
	+ 10 = Custom
	+ 11 = F

## Properties / Styles
Properties define the color, fill, etc. of elements
- Color in dxdatl.v (Type int) => Defines the line color of elements. The integer is actually the following 8 bit numbers:
	+ Red Color
	+ Green Color
	+ Blue Color
	+ Key
		* `0x00` -> Custom color
		* `0x01` -> Custom color 
		* `0xFF` -> Default color
- Style in dxdatl.v (Type int) => Defines the style of a line
	+ -1 = AutoSolid
	+ 0  = Solid
	+ 1  = Dash
	+ 2  = Center
	+ 3  = Phantom (Dash Dot Dot)
	+ 4  = Bigdash
	+ 5  = Dot
	+ 6  = DashDot
	+ 7  = Mediumdash
- Thickness in dxdatl.v (Type int) => Defines the thickness of a line. This is a number between 0 and 10.
	+ 0  = Automatic
	+ 1  = Thinnest line
	+ 2..9
	+ 10 = Thickest line
- Fill in dxdatl.v (Type int) => Defines the filling pattern of a closed shape.
	+ -1 = AutoHollow
	+ 0  = Hollow
	+ 1  = Solid
	+ 2  = Diagdn1
	+ 3  = Diagup2
	+ 4  = Grey08
	+ 5  = Diagdn2
	+ 6  = Diagup1
	+ 7  = Horiz
	+ 8  = Vert
	+ 9  = Grid2
	+ 10 = Grid1
	+ 11 = X2
	+ 12 = X1
	+ 13 = Grey50
	+ 14 = Grey92
	+ 15 = Grey04
- ColorExt in dxdatl.v (Type int) => Defines the fill color of closed shapes. The format is the same as the line color. This key does not exist on older file versions.

## Arcs
- Arc2Style in dxdatl.v (Type int) => Index into properties defining the line color, style, etc. of the arc
- ArcEnd in dxdatl.v (Type Long) => Coordinate of the arc end point (X(Int), Y(Int)) in 10nm
- ArcStart in dxdatl.v (Type Long) => Coordinate of the arc start point (X(Int), Y(Int)) in 10nm
- ArcMid in dxdatl.v (Type Long) => Coordinate of the arc center point (X(Int), Y(Int)) in 10nm
- ArcIDDxD in dxdatl.v (Type int) => Unknown
- ArcUID in dxdatl.v (Type UID) => UID of the Arc. It is used to assign the arc to the correct sheets using the grpobj file.

## Circles
- Circle2Style in dxdatl.v (Type int) => Index into properties defining the line color, style, etc. of the circle
- CircleIDDxD in dxdatl.v (Type int) => Unknown
- CenterCoord in dxdatl.v (Type Long) => Coordinate of the circle center point (X(Int), Y(Int)) in 10nm
- CircleRadius in dxdatl.v (Type int) => Radius of the circle in 10nm
- CircleUID in dxdatl.v (Type UID) => UID of the Circle. It is used to assign the circle to the correct sheets using the grpobj file.

## Lines / Polylines
Lines are made of one (Line) or multiple (Polyline) segments. 
- Line2Style in dxdatl.v (Type int) => Index into properties defining the line color, style, etc.
- LinePointX in dxdatl.v (Type int Array) => list of all line X points in 10nm. The points are NOT absolute coordinates, but always relative to the last point. The first point is referenced to 0.
- LinePointY in dxdatl.v (Type int Array) => list of all line Y points in 10nm. Same as format as LinePointX.
- LineUID in dxdatl.v (Type UID) => UID of the Line. It is used to assign the line to the correct sheets using the grpobj file.
- LineIDDxD in dxdatl.v (Type int) => Unknown
- LineArrowSize in dxdatl.v (Type int) => Size of line ending (probably same format as Thickness in Properties. Never found it anything but 0). This value is once per segment.
- LineEndingCap in dxdatl.v (Type int) => Type of line ending.. This value is once per segment. Known Endings:
	+ 0 = None
	+ 1 = SimpleArrow
	+ 2 = SimpleArrowFilled
	+ 3 = SharpArrow
	+ 4 = SharpArrowFilled
	+ 5 = ComplexArrow
	+ 6 = ComplexArrowFilled
- LineStartingCap in dxdatl.v (Type int) => Same as LineEndingCap

## Rectangles
- Rect2Style in dxdatl.v (Type int) => Index into properties defining the line color, style, etc. of the rectangle
- RectIDDxD in dxdatl.v (Type int) => Unknown
- RectCoord in dxdatl.v (Type LongLong) => Coordinate of the starting and ending point of the rectangle (X_Start(Int), Y_Start(Int), X_End(Int), Y_End(Int)) in 10nm
- RectUID in dxdatl.v (Type UID) => UID of the Rectangle. It is used to assign the rectangle to the correct sheets using the grpobj file.

## TextData
TextData define the color, font, rotation, etc. of texts
- TextColor in dxdatl.v (Type int) => Defines the fill color of a text. The format is the same as the line color in Properties
- TextColorExt in dxdatl.v (Type int) => Defines a secondary (background?) color of a text. The format is the same as the line color in Properties
- TextFont in dxdatl.v (Type string) => Defines a the font of a text. Actually a string representing either the number of an internal font, or a custom font name + formatting:
	+ For Internal fonts the codes are:
		* 0  = Fixed
		* 1  = Roman
		* 2  = RomanItalic
		* 3  = RomanBold
		* 4  = RomanBoldItalic
		* 5  = SansSerif
		* 6  = Script
		* 7  = SansSerifBold
		* 8  = ScriptBold
		* 9  = Gothic
		* 10 = OldEnglish
		* 11 = Kanji
		* 12 = Plot
	+ For custom fonts the string is multiple keys separated by "|" characters. The keys mean the following things:
		* This always 0
		* This is the font name string
		* Unknown
		* Unknown
		* This is 1 for stroked-out font formatting, otherwise 0
		* This is 1 for underlined font formatting, otherwise 0
		* This is 1 for bold font formatting, otherwise 0
		* Unknown
		* Unknown
		A text in bold formatting with font "Arial" will look like this:
		``` 0|Arial|0|0|0|0|1|0|0 ```
- TextIDDxD in dxdatl.v (Type int) => Unknown
- TextOrient in dxdatl.v (Type int) => Rotation of the text. The following rotation codes exist:
	+ 0 = 0 Degree
	+ 1 = 90 Degree
	+ 2 = 180 Degree
	+ 3 = 270 Degree
- TextOrigin in dxdatl.v (Type int) => Alignment of the text. The following alignment codes exist:
	+ 0 = Default
	+ 1 = UpperLeft
	+ 2 = MiddleLeft
	+ 3 = LowerLeft
	+ 4 = UpperCenter
	+ 5 = MiddleCenter
	+ 6 = LowerCenter
	+ 7 = UpperRight
	+ 8 = MiddleRight
	+ 9 = LowerRight
- TextPos in dxdatl.v (Type Long) => Coordinate of the text (X(Int), Y(Int)) in 10nm
- TextSize in dxdatl.v (Type int) => Size of the text in 10nm

## Texts
- TextString in dxdatl.v (Type String) => Define the text string of a graphical text element
- TextUID in dxdatl.v (Type UID) => UID of the graphical text. It is used to assign the text to the correct sheets using the grpobj file.
- Text2TextData in dxdatl.v (Type int) => Indexes into TextData defining the text formatting.

## Label
Labels are a part of nets and busses, and are indexed by them
- LabelInverted in dxdatl.v (Type int Array) => Unknown
- LabelScope in dxdatl.v (Type int Array) => Unknown
- LabelSegmentNum in dxdatl.v (Type int Array) => indexes on which net/bus segment the label is placed
- LabelVisibility in dxdatl.v (Type int Array) => defines if a label is visible or not. The following codes exist:
	+ 1 = Not visible
	+ 4 = Visible
- Label2TextData in dxdatl.v (Type int Array) => Indexes into TextData defining the text formatting.
- LabelIndexDxDNet in dxdatl.v (Type int UID) => UID of the label. Sometimes this UID is all "0". That means, that the label does not actually exist, it just names a net. The number of entry’s does sometimes not match the other entry’s. Not jet sure why.
- LabelIDDxD in dxdatl.v (Type int) => Unknown

## Joint
Joints mark the start, end, and branching points of nets and busses. They are indexed by segments to define which joints are connected.
- JointPos in dxdatl.v (Type Long) => Position of the joint (X(Int), Y(Int)) in 10nm.
- Joint2UID in dxdatl.v (Type UID) => UID of the joint.
- JointID in dxdatl.v (Type int) => Unknown

## Segments
Segments are the connection between multiple joints. They are indexed by nets and busses to define their actual graphical connections.
- Segments in dxdatl.v (Type int array) => List of Segment (singular). Every Segments type marks a different net or bus. The first entry of the list is the start index into Segment2Joints plus 1. Every subsequent Segment2Joints index is the entry plus the index of the last Segment2Joints plus 1.
- Segment2Joints in dxdatl.v (Type Long) => Where Segments indexes into. This defines the start and end point (Start(Int), End(Int)) index of a net or bus segment indexing into Joints.  
- SegmentID in dxdatl.v (Type int) => Unknown

## Net
There are multiple Net keys meaning different things:
+ NetDxD => Unique Nets (Not graphically but electrically connected nets have one entry per unique graphical net)
+ Net => Glogal Nets (Not graphically but electrically connected nets have one single entry)
+ NetsDXD => Page Nets (Not graphically but electrically connected nets have one single entry per page they appear on)

These keys are known:
- NetDxD2Net in dxdatl.v (Type int) => Indexes NetDxD into Net. 
- NetDxDUID in dxdatl.v (Type UID) => UID of the Net. It is used to assign the net to the correct sheets using the grpobj file.
- NetID in dxdatl.v (Type int) => Unknown
- NetLabel in dxdatl.v (Type String) => Define the net name / label string
- Net2GrpLabels in dxdatl.v (Type int Array) => Indexes into Labels to add the net label if the exist.
- Net2GrpSegments in dxdatl.v (Type int Array) => Indexes into Segments to add the net lines
- Net2GrpStyles in dxdatl.v (Type int Array) => Index into properties defining the line color of the net

## Bus
- BusNam in blkatl.v (Type String) => Define the bus name / label string
- BusUID in blkatl.v (Type UID) => UID of the bus. It is used to assign the bus to the correct sheets using the grpobj file.
- BusID in dxdatl.v (Type int) => Unknown
- BusType in blkatl.v (Type int) => Unknown also not present in some older versions
- Bus2GrpLabels in dxdatl.v (Type int Array) => Indexes into Labels to add the bus label. Buses without custom names don't have a label entry, leading to a mismatch between Bus2GrpSegment and the other bus data. Buses without custom names ("$" Prefix) can be skipped.
- Bus2GrpSegments in dxdatl.v (Type int Array) => Indexes into Segments to add the bus lines
- Bus2Grps in dxdatl.v (Type int Array) => Unknown
- BusID in dxdatl.v (Type int Array) => Unknown