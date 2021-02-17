#ifndef JMM_CLASS_FILE_HPP
#define JMM_CLASS_FILE_HPP
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Definitions.hpp"

// cp_info {
//   u1 tag;
//   u1 info[];
// }
struct CPInfo {
 public:
  virtual std::string info() = 0;
};

// CONSTANT_Class_info {
//     u1 tag;
//     u2 name_index;
// }
struct ConstantClass : public CPInfo {
 public:
  static const uint8_t tagValue = 7;
  uint16_t nameIndex;
  virtual std::string info() override;
};

// CONSTANT_Fieldref_info {
//     u1 tag;
//     u2 class_index;
//     u2 name_and_type_index;
// }
struct ConstantFieldRef : public CPInfo {
 public:
  static const uint8_t tagValue = 9;
  uint16_t classIndex;
  uint16_t nameAndTypeIndex;
  virtual std::string info() override;
};

// CONSTANT_Methodref_info {
//     u1 tag;
//     u2 class_index;
//     u2 name_and_type_index;
// }
struct ConstantMethodRef : public CPInfo {
 public:
  static const uint8_t tagValue = 10;
  uint16_t classIndex;
  uint16_t nameAndTypeIndex;
  virtual std::string info() override;
};

// CONSTANT_InterfaceMethodref_info {
//     u1 tag;
//     u2 class_index;
//     u2 name_and_type_index;
// }
struct ConstantInterfaceMethodRef : public CPInfo {
 public:
  static const uint8_t tagValue = 11;
  uint16_t classIndex;
  uint16_t nameAndTypeIndex;
  virtual std::string info() override;
};

// CONSTANT_String_info {
//     u1 tag;
//     u2 string_index;
// }
struct ConstantString : public CPInfo {
 public:
  static const uint8_t tagValue = 8;
  uint16_t stringIndex;
  virtual std::string info() override;
};

// CONSTANT_Integer_info {
//     u1 tag;
//     u4 bytes;
// }
struct ConstantInteger : public CPInfo {
 public:
  static const uint8_t tagValue = 3;
  uint32_t bytes;
  virtual std::string info() override;
};
// CONSTANT_Float_info {
//     u1 tag;
//     u4 bytes;
// }
struct ConstantFloat : public CPInfo {
 public:
  static const uint8_t tagValue = 4;
  uint32_t bytes;
  virtual std::string info() override;
};

// CONSTANT_Long_info {
//     u1 tag;
//     u4 high_bytes;
//     u4 low_bytes;
// }
struct ConstantLong : public CPInfo {
 public:
  static const uint8_t tagValue = 5;
  uint32_t highBytes;
  uint32_t lowBytes;
  virtual std::string info() override;
};

// CONSTANT_Double_info {
//     u1 tag;
//     u4 high_bytes;
//     u4 low_bytes;
// }
struct ConstantDouble : public CPInfo {
 public:
  static const uint8_t tagValue = 6;
  uint32_t highBytes;
  uint32_t lowBytes;
  virtual std::string info() override;
};

// CONSTANT_NameAndType_info {
//     u1 tag;
//     u2 name_index;
//     u2 descriptor_index;
// }
struct ConstantNameAndType : public CPInfo {
 public:
  static const uint8_t tagValue = 12;
  uint16_t nameIndex;
  uint16_t descriptorIndex;
  virtual std::string info() override;
};

// CONSTANT_Utf8_info {
//     u1 tag;
//     u2 length;
//     u1 bytes[length];
// }
struct ConstantUtf8 : public CPInfo {
 public:
  static const uint8_t tagValue = 1;
  std::string bytes;
  virtual std::string info() override;
};

// CONSTANT_MethodHandle_info {
//     u1 tag;
//     u1 reference_kind;
//     u2 reference_index;
// }
struct ConstantMethodHandle : public CPInfo {
 public:
  static const uint8_t tagValue = 15;
  uint8_t referenceKind;
  uint16_t referenceIndex;
  virtual std::string info() override;
};

// CONSTANT_MethodType_info {
//     u1 tag;
//     u2 descriptor_index;
// }
struct ConstantMethodType : public CPInfo {
 public:
  static const uint8_t tagValue = 16;
  uint16_t descriptorIndex;
  virtual std::string info() override;
};

// CONSTANT_InvokeDynamic_info {
//     u1 tag;
//     u2 bootstrap_method_attr_index;
//     u2 name_and_type_index;
// }
struct ConstantInvokeDynamic : public CPInfo {
 public:
  static const uint8_t tagValue = 18;
  uint16_t bootstrapMethodAttrIndex;
  uint16_t nameAndTypeIndex;
  virtual std::string info() override;
};

// attribute_info {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u1 info[attribute_length];
// }
class AttributeInfo {};

// ConstantValue_attribute {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u2 constantvalue_index;
// }
class ConstantValueAttribute : public AttributeInfo {
 public:
  uint16_t constantValueIndex;
  static const std::string attributeName;
};

// exception_table
// u2 start_pc;
// u2 end_pc;
// u2 handler_pc;
// u2 catch_type;
struct ExceptionEntry {
  uint16_t startPC;
  uint16_t endPC;
  uint16_t handlerPC;
  uint16_t catchType;
};

// Code_attribute {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u2 max_stack;
//     u2 max_locals;
//     u4 code_length;
//     u1 code[code_length];
//     u2 exception_table_length;
//     {   u2 start_pc;
//         u2 end_pc;
//         u2 handler_pc;
//         u2 catch_type;
//     } exception_table[exception_table_length];
//     u2 attributes_count;
//     attribute_info attributes[attributes_count];
// }
class CodeAttribute : public AttributeInfo {
 public:
  uint16_t maxStack;
  uint16_t maxLocals;
  std::vector<uint8_t> code;
  std::vector<ExceptionEntry> exceptionTable;
  std::map<std::string, AttributeInfo*> attributes;
  static const std::string attributeName;
};

enum VerificationType : uint8_t {
  TopVerification = 0,
  IntegerVerification = 1,
  FloatVerification = 2,
  LongVerification = 4,
  DoubleVerification = 3,
  NullVerification = 5,
  UninitializedThisVerification = 6,
  ObjectVerification = 7,
  UninitializedVerification = 8
};

struct VerificationInfo {
  VerificationType tag;
  uint16_t cPoolIndexOrOffset;
};

enum StackMapFrameType {
  // same_frame {
  // u1 frame_type = SAME; /* 0-63 */
  // }
  Same,
  // same_locals_1_stack_item_frame {
  // u1 frame_type = SAME_LOCALS_1_STACK_ITEM; /* 64-127 */
  // verification_type_info stack[1];
  // }
  SameLocals,
  // same_locals_1_stack_item_frame_extended {
  // u1 frame_type = SAME_LOCALS_1_STACK_ITEM_EXTENDED; /* 247 */
  // u2 offset_delta;
  // verification_type_info stack[1];
  // }
  SameLocalsExtended,
  // chop_frame {
  // u1 frame_type = CHOP; /* 248-250 */
  // u2 offset_delta;
  // }
  Chop,

  // same_frame_extended {
  // u1 frame_type = SAME_FRAME_EXTENDED; /* 251 */
  // u2 offset_delta;
  // }
  SameExtended,
  // append_frame {
  // u1 frame_type = APPEND; /* 252-254 */
  // u2 offset_delta;
  // verification_type_info locals[frame_type - 251];
  // }
  Append,
  //  full_frame {
  // u1 frame_type = FULL_FRAME; /* 255 */
  // u2 offset_delta;
  // u2 number_of_locals;
  // verification_type_info locals[number_of_locals];
  // u2 number_of_stack_items;
  // verification_type_info stack[number_of_stack_items];
  // }
  Full
};

struct StackMapFrame {
  StackMapFrameType type;
  uint16_t offsetDelta;
  std::vector<VerificationInfo> locals;
  std::vector<VerificationInfo> stack;
};

// StackMapTable_attribute
//  u2              attribute_name_index;
//  u4              attribute_length;
//  u2              number_of_entries;
//  stack_map_frame entries[number_of_entries];
//
class StackMapTableAttribute : public AttributeInfo {
 public:
  std::vector<StackMapFrame> entries;
  static const std::string attributeName;
};

// SourceFile_attribute {
//        u2 attribute_name_index;
//        u4 attribute_length;
//        u2 sourcefile_index;
// }
class SourceFileAttribute : public AttributeInfo {
 public:
  uint16_t sourceFileIndex;
  static const std::string attributeName;
};

struct BootstrapMethod {
  uint16_t methodRef;
  std::vector<uint16_t> arguments;
};
// BootstrapMethods_attribute {
//        u2 attribute_name_index;
//        u4 attribute_length;
//        u2 num_bootstrap_methods;
//        {   u2 bootstrap_method_ref;
//            u2 num_bootstrap_arguments;
//            u2 bootstrap_arguments[num_bootstrap_arguments];
//        } bootstrap_methods[num_bootstrap_methods];
// }
class BootstrapMethodsAttribute : public AttributeInfo {
 public:
  std::vector<BootstrapMethod> bootstrapMethods;
  static const std::string attributeName;
};

// NestHost_attribute {
//        u2 attribute_name_index;
//        u4 attribute_length;
//        u2 host_class_index;
// }
class NestHostAttribute : public AttributeInfo {
 public:
  uint16_t hostClassIndex;
  static const std::string attributeName;
};

// NestMembers_attribute {
//        u2 attribute_name_index;
//        u4 attribute_length;
//        u2 number_of_classes;
//        u2 classes[number_of_classes];
// }
class NestMembersAttribute : public AttributeInfo {
 public:
  std::vector<uint16_t> classes;
  static const std::string attributeName;
};

// field_info {
//     u2             access_flags;
//     u2             name_index;
//     u2             descriptor_index;
//     u2             attributes_count;
//     attribute_info attributes[attributes_count];
// }
struct FieldInfo {
  uint16_t accsessFlag;
  uint16_t nameIndex;
  uint16_t descriptorIndex;
  std::map<std::string, AttributeInfo*> attributes;
};

// method_info {
//     u2             access_flags;
//     u2             name_index;
//     u2             descriptor_index;
//     u2             attributes_count;
//     attribute_info attributes[attributes_count];
// }
struct MethodInfo {
  uint16_t accsessFlag;
  uint16_t nameIndex;
  uint16_t descriptorIndex;
  std::map<std::string, AttributeInfo*> attributes;
};

// ClassFile {
//     u4             magic;
//     u2             minor_version;
//     u2             major_version;
//     u2             constant_pool_count;
//     cp_info        constant_pool[constant_pool_count-1];
//     u2             access_flags;
//     u2             this_class;
//     u2             super_class;
//     u2             interfaces_count;
//     u2             interfaces[interfaces_count];
//     u2             fields_count;
//     field_info     fields[fields_count];
//     u2             methods_count;
//     method_info    methods[methods_count];
//     u2             attributes_count;
//     attribute_info attributes[attributes_count];
// }
struct ClassFile {
  uint32_t magic;
  uint16_t minorVersion;
  uint16_t majorVersion;
  //   uint16_t constant_pool_count;
  std::map<size_t, CPInfo*> constantPool;
  uint16_t accessFlags;
  uint16_t thisClass;
  uint16_t superClass;
  //   uint16_t interfacesCount;
  std::vector<uint16_t> interfaces;
  //   uint16_t fields_count;
  std::vector<FieldInfo> fields;
  //   uint16_t methods_count;
  std::vector<MethodInfo> methods;
  //   uint16_t attributes_count;
  std::map<std::string, AttributeInfo*> attributes;
  void printContents();
};

#endif  // JMM_CLASS_FILE_HPP