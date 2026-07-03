meta:
  id: icdb_uids
  application: DXDesigner/icdbServer_extradcted
  endian: le

seq:
- id: key # first entries
  type: header
  repeat: eos
  eos-error: true

types:
  header:
    seq:
    - id: len
      type: u4
    - id: entrydata
      type: entry
      repeat: expr
      repeat-expr: len
  entry:
    seq:
    - id: typecode
      type: u1
    - id: uid_owner # UID_Owner
      type: u1
    - id: uid_id_lsb
      type: u2
      if: typecode == 0x71 or typecode == 0x70
    #- id: data3_1
    #  size: 3
    #  if: typecode == 0x70
    - id: data4
      type: u2
      valid: 0x0002 # have never seen any other code
      if: typecode == 0xE4
    - id: id
      size: 2
      if: typecode == 0x64
    - id: uid_type
      type: u1
      enum: uid_types
      if: typecode == 0x71 or typecode == 0x70 or typecode == 0x64
    - id: padding
      size: 2
      if: typecode == 0x71 or typecode == 0x70
    - id: unknown
      size: 3
      if: typecode == 0x64
    - id: data5
      size: 7
      if: typecode == 0x65
    - id: data7
      size: 10
      if: typecode == 0x67
    - id: data1
      size: 3
      if: typecode == 0xB1
    - id: data3_0
      size: 4
      if: typecode == 0xA1
    - id: data2
      size: 4
      if: typecode == 0xA4
    - id: data3_2
      size: 5
      if: typecode == 0xA5
    - id: data3_3
      size: 8
      if: typecode == 0xA7
    - id: data3_4
      size: 10
      if: typecode == 0x87
    - id: data3_5
      size: 12
      if: typecode == 0x47
    - id: data3_6
      size: 6
      if: typecode == 0x61
    - id: data3_7
      size: 4
      if: typecode == 0xF1
    - id: data3_8
      size: 7
      if: typecode == 0x51
enums:
  uid_types:
    0x03: uid_type_cnfg
    0x04: uid_type_path
    0x05: uid_type_blk
    0x06: uid_type_cmp
    0x07: uid_type_bsym
    0x08: uid_type_isym
    0x09: uid_type_pin
    0x0A: uid_type_bpin
    0x0B: uid_type_ipin
    0x0C: uid_type_inet
    0x0D: uid_type_fnet
    0x0E: uid_type_pnet
    0x0F: uid_type_xtr
    0x10: uid_type_bus
    0x15: uid_type_design
    0x16: uid_type_const
    0x17: uid_type_layer
    0x18: uid_type_netclass
    0x19: uid_type_clearrule
    0x1A: uid_type_constraintclass
    0x1B: uid_type_viaspan
    0x1C: uid_type_genclearclass
    0x1D: uid_type_parttech
    0x1F: uid_type_c2cclear
    0x25: uid_type_scheme
    0x26: uid_type_schemenetclassrule
    0x27: uid_type_viaassign
    0x28: uid_type_layerruleuid
    0x29: uid_type_schemaclearrule
    0x2A: uid_type_layerclearrule
    0x2B: uid_type_genobjtype
    0x2C: uid_type_gencompobj
    0x2E: uid_type_diffpair
    0x2F: uid_type_electricalnet
    0x30: uid_type_pinpair
    0x31: uid_type_physicalnet
    0x32: uid_type_fromto
    0x33: uid_type_component
    0x34: uid_type_cespin
    0x35: uid_type_part
    0x36: uid_type_partpin
    0x37: uid_type_netdxd
    0x38: uid_type_line
    0x39: uid_type_circle
    0x3A: uid_type_arc
    0x3B: uid_type_rectangle
    0x3C: uid_type_text
    0x3E: uid_type_via
    0x3F: uid_type_cesconfig
    0x45: uid_type_group