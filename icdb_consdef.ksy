meta:
  id: icdb_consdef
  application: DXDesigner/icdbServer_extradcted
  endian: le

seq:
- id: version # version string
  type: name
- id: num_keys # number of entries
  type: u4
- id: keys # first entries
  type: entry
  repeat: expr
  repeat-expr: num_keys
- id: num_otherkey # number of 2nd entries
  type: u4
- id: otherkey # 2nd entries
  type: otherentry
  repeat: expr
  repeat-expr: num_otherkey
- id: num_voltrefs  # number of voltage reference entries
  type: u4
- id: voltrefs # voltage reference entries
  type: name
  repeat: expr
  repeat-expr: num_voltrefs

types:

  name:
    seq:
    - id: len
      type: u4
    - id: value
      type: strz
      encoding: ASCII
      size: len

  data:
    seq:
    - id: len
      type: u4
    - id: data
      type: strz
      size: len * 4
      encoding: ASCII

  entry:
    seq:
    - id: index
      type: u4
    - id: entry_name
      type: name
    - id: entry_data
      type: data

  otherentry:
    seq:
    - id: index # index of this entry
      type: u4
    - id: value1
      type: u4
    - id: index1 # again?
      type: u2
    - id: value2
      type: u1
    - id: magic # always 72?
      type: u1
      valid:
        eq: 72
    - id: value3
      type: u4
    - id: value4
      type: u4
    - id: value5
      type: name
    - id: name # name of this entry
      type: name
    - id: value6
      type: data
    - id: value7
      type: data