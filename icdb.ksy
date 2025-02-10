meta:
  id: icdb
  application: DXDesigner/icdbServer
  file-extension: dat
  endian: le

seq:
  - id: database_header
    type: database_header_type

types:
  name:
    seq:
    - id: len
      type: u4
    - id: value
      type: strz
      encoding: ASCII
      size: len

  guid_type:
    doc: |  Format: Byte 0-7 = First Part, 8-15 = Third Part, 16-23 Second Part. Value in hex with the nibbles of each byte swapped
    seq:
    - id: guid1
      size: 8
    - id: guid3
      size: 8
    - id: guid2
      size: 8

  database_header_type:
    seq:
      - id: file_version # Version of iCDB file. Current version = 1009, others might not decode correctly
        type: u4

      - id: icdb_version # Version of iCDB Server used to create the file. 2.10 = 4000, 2.11 = 4100...
        type: u4

      - id: project_guid # Project GUID
        type: guid_type

      - id: server_guid # Server GUID
        type: guid_type

      - id: opening_cnt # Opening Counter? Not 100% sure
        type: u4

      - id: always_zero # Always zero?
        type: u4

      - id: unknown1 # Unknown, seems like a part of a GUID
        type: u4

      - id: unknown3 # Unknown. Both seems to always hold the same data
        type: u4
        repeat: expr
        repeat-expr: 2

      - id: always_zero2 # Zero?
        type: u4

      - id: num_files # Total number of files in this db
        type: u4

      - id: first_list # Address of the first file list
        type: u4

      - id: num_lists # Total number of file lists
        type: u4

      - id: unknown2 # Zero most of the time, sometimes different value
        type: u4

      - id: filesize # Total file size
        type: u4
      
      - id: padding # Zero padding? Version block starts at address 0x085C
        size: 0x7F8

        # This block starts at address 0x085C
      - id: icdbdiagnostic # iCDB Server diagnostic info
        #pos: 0x085C
        type: name

      - id: pc_name # Name of the last pc it was last edited with
        type: name
  
      - id: user_name # Windows username of the last designer
        type: name
  
      - id: edittime # Last edit time
        type: u4 # Actually time32_t
  
      - id: pid # User PID of the last designer
        type: u4

      - id: os_version # Windows os info
        type: name

      - id: icdb_string # iCDB string. always "iCDB Server"?
        type: name

      - id: filepath # Filepath of this file on the last edit
        type: name

      - id: settingspath # Settings path string
        type: name

      - id: file_list
        type: file_list_type(_index)
        repeat: until
        repeat-until: _.next_file_list == 0

  file_list_type:
    params:
      - id: index
        type: u4

    instances:
      block_address:
        value: 'index > 0 ? _parent.file_list[index - 1].next_file_list : _parent.first_list'

      file_cnt: # Number of files in this file list, Max 100 files per list
        type: u1
        io: _root._io
        pos: block_address

      always_zero: # Always zero?
        type: u1
        repeat: expr
        repeat-expr: 4
        pos: block_address + 1

      file_cnt2: # Again?
        type: u1
        pos: block_address + 5

      always_zero2: # Always zero?
        type: u2
        pos: block_address + 6

      magic: # Always 0x6410?
        type: u4
        pos: block_address + 8
        valid:
          eq: 0x6410

      next_file_list: # Start address of the next list, 0 if there is no next list
        type: u4
        pos: block_address + 12

      file:
        pos: block_address + 16
        type: file_type
        repeat: expr
        repeat-expr: file_cnt

  file_type:
    seq:
      - id: file_address # Start address of this file (address of this byte)
        type: u4

      - id: always_zero # Always zero?
        type: u4

      - id: filename_length # Settings path string length
        type: u4
      - id: value # Settings path 
        type: strz
        encoding: ASCII
        size: filename_length
      - id: filename_zeropadding # Padding to 160 character
        type: strz
        encoding: ASCII
        size: 160-filename_length

      - id: data # Sometimes data in human readable format
        type: str
        encoding: ASCII
        size: 0x10

      - id: otherdata # Always zero?
        type: u4
        repeat: expr
        repeat-expr: 2

      - id: zeropadding # Zeros
        type: u4

      - id: unknown # Unknown number
        type: u4

      - id: guid # File GUID
        type: guid_type

      - id: data_size # Size of file payload
        type: u4

      - id: data_address # Starting address of payload
        type: u4

      - id: one # Unknown. 1 most of the time?
        type: u4
        repeat: expr
        repeat-expr: 2

      - id: unknown2 # Unknown
        type: u4
        repeat: expr
        repeat-expr: 2

      - id: padded_file_length # Each file entry is always a multiple of 64 bytes in size, unused space is padded with 0
        type: u4

      - id: fragment # Data fragment
        doc: | files starting with 0xXX 0xfd 0xff 0xff 0x01 mark z-lib compressed data
        type: fragment_type(_index)
        repeat: until
        repeat-until: _.next_fragment == 0

  fragment_type:
    params:
      - id: index
        type: u4

    instances:
      block_address:
        value: 'index > 0 ? _parent.fragment[index - 1].next_fragment : _parent.data_address'

      payload_length: # Length of payload in this fragment
        type: u4
        io: _root._io
        pos: block_address

      fragment_length: # Total length of this fragment (Header + Payload + Padding)
        type: u4
        pos: block_address + 4

      dublicates: # Number of files containing the same fragment of data
        type: u4
        pos: block_address + 8

      next_fragment: # Start address of the next fragment, 0 if there is no next fragment
        type: u4
        pos: block_address + 12

      payload: # Data payload
        pos: block_address + 16
        type: str
        encoding: ASCII
        size: payload_length