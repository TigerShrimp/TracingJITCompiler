#ifndef JMM_CLASS_FILE_HPP
#define JMM_CLASS_FILE_HPP
#include <iostream>
#include <map>
#include <string>
#include <vector>

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
  uint16_t startPc;
  uint16_t endPc;
  uint16_t handlerPc;
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

enum VerificationType {
  Top,
  Integer,
  Float,
  Long,
  Double,
  Null,
  UninitializedThis,
  Object,
  Uninitialized
};

struct VerificationInfo {
  VerificationType tag;
  uint16_t cPoolIndexOrOffset;
};

enum StackMapFrameType {
  Same,
  SameLocals,
  SameLocalsExtended,
  Chop,
  SameExtended,
  Append,
  Full
};

struct StackMapFrame {
  StackMapFrameType type;
  uint16_t offsetData;
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

// class ExceptionsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class InnerClassesAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class EnclosingMethodAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class SyntheticAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class SignatureAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };

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
// class SourceDebugExtensionAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };

// line_number_table
//  u2 start_pc;
//  u2 line_number;
struct LineNumberEntry {
  uint16_t startPC;
  uint16_t lineNumber;
};

// LineNumberTable_attribute {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u2 line_number_table_length;
//     {   u2 start_pc;
//         u2 line_number;
//     } line_number_table[line_number_table_length];
// }
class LineNumberTableAttribute : public AttributeInfo {
 public:
  std::vector<LineNumberEntry> lineNumberTable;
  static const std::string attributeName;
};

// local_variable_table
// u2 start_pc;
// u2 length;
// u2 name_index;
// u2 descriptor_index;
// u2 index;
struct LocalVariableEntry {
  uint16_t startPC;
  uint16_t length;
  uint16_t nameIndex;
  uint16_t descriptorIndex;
  uint16_t index;
};

// LocalVariableTable_attribute {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u2 local_variable_table_length;
//     {   u2 start_pc;
//         u2 length;
//         u2 name_index;
//         u2 descriptor_index;
//         u2 index;
//     } local_variable_table[local_variable_table_length];
// }
class LocalVariableTableAttribute : public AttributeInfo {
 public:
  std::vector<LocalVariableEntry> localVariableTable;
  static const std::string attributeName;
};

// local_variable_type_table
// u2 start_pc;
// u2 length;
// u2 name_index;
// u2 signature_index;
// u2 index;
struct LocalVariableTypeEntry {
  uint16_t startPC;
  uint16_t length;
  uint16_t nameIndex;
  uint16_t signatureIndex;
  uint16_t index;
};

// LocalVariableTypeTable_attribute {
//     u2 attribute_name_index;
//     u4 attribute_length;
//     u2 local_variable_type_table_length;
//     {   u2 start_pc;
//         u2 length;
//         u2 name_index;
//         u2 signature_index;
//         u2 index;
//     } local_variable_type_table[local_variable_type_table_length];
// }
class LocalVariableTypeTableAttribute : public AttributeInfo {
 public:
  std::vector<LocalVariableTypeEntry> localVariableTypeTable;
  static const std::string attributeName;
};
// class DeprecatedAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class RuntimeVisibleAnnotationsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class RuntimeInvisibleAnnotationsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class RuntimeVisibleParameterAnnotationsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class RuntimeInvisibleParameterAnnotationsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class AnnotationDefaultAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };
// class BootstrapMethodsAttribute : public AttributeInfo {
//  public:
//   virtual static std::string attributeName() override;
// };

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
  std::vector<CPInfo*> constantPool;
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