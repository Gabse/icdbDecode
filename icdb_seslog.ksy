meta:
  id: icdb_seslog
  application: DXDesigner/icdbServer_extradcted
  endian: le
seq:
  - id: file_header
    type: file_header_type
  - id: entry_header
    type: entry_header_type
    repeat: eos
types:
  file_header_type:
    seq:
    - id: magic # always 1000
      type: u4
      valid:
        eq: 1000

    - id: some_number # number depending on number of entries. not quite shure how it is calculated
      type: u4

    - id: another_number # not shure what 
      type: u4
  entry_header_type:
    seq:
    - id: magic # always 1000
      type: u4
      valid:
        eq: 1000

    - id: entry_time # date of creation?
      type: u4

    - id: version # version code?
      type: strz
      encoding: ASCII
      
    - id: editor_version_string_short # editor version code?
      type: strz
      encoding: ASCII
      
    - id: editor_version_string_long # longer version
      type: strz
      encoding: ASCII
      
    - id: editor_build # build number?
      type: strz
      encoding: ASCII
      
    - id: editor_compile # editor compile date?
      type: strz
      encoding: ASCII
      
    - id: editor_source # editor source stamp?
      type: strz
      encoding: ASCII
    
    - id: editor_version # version as number
      type: u4
      
    - id: server_version # version as number
      type: u4
      
    - id: mashine # mashine that created the entry
      type: strz
      encoding: ASCII
      
    - id: user # user that created the entry
      type: strz
      encoding: ASCII
      
    - id: pid #  user pid that created the entry? not shure
      type: u4
      
    - id: os # os that created the entry
      type: strz
      encoding: ASCII
      
    - id: project_guid # Project GUID?
      size: 24
      
    - id: server_guid # Server GUID?
      size: 24
      
    - id: entry1 # entry number?
      type: u4
      
    - id: entry2 # again?
      type: u4
      
    - id: filepath # filepath of this file on the last edit
      type: strz
      encoding: ASCII
      
    - id: settingspath # settings path string
      type: strz
      encoding: ASCII