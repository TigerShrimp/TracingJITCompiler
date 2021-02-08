#include "JVM/Parser.hpp"

ClassFile Parser::parse(std::string classFilePath) {
  std::ifstream input(classFilePath, std::ios::binary);

  std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                             (std::istreambuf_iterator<char>()));

  input.close();
  return parse(bytes);
}

ClassFile Parser::parse(std::vector<uint8_t> classBytes) {
  std::vector<uint8_t>::iterator classCursor = classBytes.begin();
  std::vector<CPInfo*> cpInfo;
  //   ClassFile {
  ClassFile cf{
      //     u4             magic;
      readU4(classCursor),
      //     u2             minor_version;
      readU2(classCursor),
      //     u2             major_version;
      readU2(classCursor),
      //     u2             constant_pool_count;
      //     cp_info        constant_pool[constant_pool_count-1];
      (cpInfo = parseConstantPoolInfo(classCursor)),
      //     u2             access_flags;
      readU2(classCursor),
      //     u2             this_class;
      readU2(classCursor),
      //     u2             super_class;
      readU2(classCursor),
      //     u2             interfaces_count;
      //     u2             interfaces[interfaces_count];s
      parseInterfaces(classCursor),
      //     u2             fields_count;
      //     field_info     fields[fields_count];
      parseFields(classCursor, cpInfo),
      //     u2             methods_count;
      //     method_info    methods[methods_count];
      parseMethods(classCursor, cpInfo),
      //     u2             attributes_count;
      //     attribute_info attributes[attributes_count];
      parseAttributeInfo(classCursor, cpInfo)
      // }
  };
  assert(classCursor == classBytes.end());
  return cf;
}

std::vector<CPInfo*> Parser::parseConstantPoolInfo(
    std::vector<uint8_t>::iterator& classCursor) {
  uint16_t constantPoolCount = readU2(classCursor);

  std::vector<CPInfo*> cpInfo;
  cpInfo.push_back(new ConstantClass());
  for (uint16_t i = 1; i < constantPoolCount; i++) {
    uint8_t tag = readU1(classCursor);
    switch (tag) {
      case ConstantClass::tagValue: {
        ConstantClass* constantClass = new ConstantClass();
        constantClass->nameIndex = readU2(classCursor);
        cpInfo.push_back(constantClass);
        break;
      }
      case ConstantFieldRef::tagValue: {
        ConstantFieldRef* constantFieldRef = new ConstantFieldRef();
        constantFieldRef->classIndex = readU2(classCursor);
        constantFieldRef->nameAndTypeIndex = readU2(classCursor);
        cpInfo.push_back(constantFieldRef);
        break;
      }
      case ConstantMethodRef::tagValue: {
        ConstantMethodRef* constantMethodRef = new ConstantMethodRef();
        constantMethodRef->classIndex = readU2(classCursor);
        constantMethodRef->nameAndTypeIndex = readU2(classCursor);
        cpInfo.push_back(constantMethodRef);
        break;
      }
      case ConstantInterfaceMethodRef::tagValue: {
        ConstantInterfaceMethodRef* constantInterfaceMethodRef =
            new ConstantInterfaceMethodRef();
        constantInterfaceMethodRef->classIndex = readU2(classCursor);
        constantInterfaceMethodRef->nameAndTypeIndex = readU2(classCursor);
        cpInfo.push_back(constantInterfaceMethodRef);
        break;
      }
      case ConstantString::tagValue: {
        ConstantString* constantString = new ConstantString();
        constantString->stringIndex = readU2(classCursor);
        cpInfo.push_back(constantString);
        break;
      }
      case ConstantInteger::tagValue: {
        ConstantInteger* constantInteger = new ConstantInteger();
        constantInteger->bytes = readU4(classCursor);
        cpInfo.push_back(constantInteger);
        break;
      }
      case ConstantFloat::tagValue: {
        ConstantFloat* constantFloat = new ConstantFloat();
        constantFloat->bytes = readU4(classCursor);
        cpInfo.push_back(constantFloat);
        break;
      }
      case ConstantLong::tagValue: {
        ConstantLong* constantLong = new ConstantLong();
        constantLong->highBytes = readU4(classCursor);
        constantLong->lowBytes = readU4(classCursor);
        cpInfo.push_back(constantLong);
        break;
      }
      case ConstantDouble::tagValue: {
        ConstantDouble* constantDouble = new ConstantDouble();
        constantDouble->highBytes = readU4(classCursor);
        constantDouble->lowBytes = readU4(classCursor);
        cpInfo.push_back(constantDouble);
        break;
      }
      case ConstantNameAndType::tagValue: {
        ConstantNameAndType* constantNameAndType = new ConstantNameAndType();
        constantNameAndType->nameIndex = readU2(classCursor);
        constantNameAndType->descriptorIndex = readU2(classCursor);
        cpInfo.push_back(constantNameAndType);
        break;
      }
      case ConstantUtf8::tagValue: {
        ConstantUtf8* constantUtf8 = new ConstantUtf8();
        uint16_t length = readU2(classCursor);
        constantUtf8->bytes = std::string(classCursor, classCursor + length);
        advance(classCursor, length);
        cpInfo.push_back(constantUtf8);
        break;
      }
      case ConstantMethodHandle::tagValue: {
        ConstantMethodHandle* constantMethodHandle = new ConstantMethodHandle();
        constantMethodHandle->referenceKind = readU1(classCursor);
        constantMethodHandle->referenceIndex = readU2(classCursor);
        cpInfo.push_back(constantMethodHandle);
        break;
      }
      case ConstantMethodType::tagValue: {
        ConstantMethodType* constantMethodType = new ConstantMethodType();
        constantMethodType->descriptorIndex = readU2(classCursor);
        cpInfo.push_back(constantMethodType);
        break;
      }
      case ConstantInvokeDynamic::tagValue: {
        ConstantInvokeDynamic* constantInvokeDynamic =
            new ConstantInvokeDynamic();
        constantInvokeDynamic->bootstrapMethodAttrIndex = readU2(classCursor);
        constantInvokeDynamic->nameAndTypeIndex = readU2(classCursor);
        cpInfo.push_back(constantInvokeDynamic);
        break;
      }
      default:
        std::cout << "Unrecognized tag" << tag << std::endl;
        break;
    }
  }
  return cpInfo;
}

std::vector<uint16_t> Parser::parseInterfaces(
    std::vector<uint8_t>::iterator& classCursor) {
  uint16_t interfacesCount = readU2(classCursor);
  std::vector<uint16_t> interfaces;
  for (int i = 0; i < interfacesCount; i++) {
    interfaces.push_back(readU2(classCursor));
  }
  return interfaces;
}

std::vector<FieldInfo> Parser::parseFields(
    std::vector<uint8_t>::iterator& classCursor, std::vector<CPInfo*> cpInfo) {
  uint16_t fieldsCount = readU2(classCursor);
  std::vector<FieldInfo> fields;
  for (int i = 0; i < fieldsCount; i++) {
    FieldInfo fieldInfo;
    fieldInfo.accsessFlag = readU2(classCursor);
    fieldInfo.nameIndex = readU2(classCursor);
    fieldInfo.descriptorIndex = readU2(classCursor);
    fieldInfo.attributes = parseAttributeInfo(classCursor, cpInfo);
    fields.push_back(fieldInfo);
  }
  return fields;
}
std::vector<MethodInfo> Parser::parseMethods(
    std::vector<uint8_t>::iterator& classCursor, std::vector<CPInfo*> cpInfo) {
  uint16_t methodsCount = readU2(classCursor);
  std::vector<MethodInfo> methods;
  for (int i = 0; i < methodsCount; i++) {
    MethodInfo methodInfo;
    methodInfo.accsessFlag = readU2(classCursor);
    methodInfo.nameIndex = readU2(classCursor);
    methodInfo.descriptorIndex = readU2(classCursor);
    methodInfo.attributes = parseAttributeInfo(classCursor, cpInfo);
    methods.push_back(methodInfo);
  }
  std::cout << "Methods parsed:" << methods.size() << std::endl;
  return methods;
}

std::map<std::string, AttributeInfo*> Parser::parseAttributeInfo(
    std::vector<uint8_t>::iterator& classCursor, std::vector<CPInfo*>& cpInfo) {
  const uint16_t attributesCount = readU2(classCursor);
  std::map<std::string, AttributeInfo*> attributes;
  for (int i = 0; i < attributesCount; i++) {
    AttributeInfo* ai = NULL;
    uint16_t attributeNameIndex = readU2(classCursor);
    ConstantUtf8* attributeName = (ConstantUtf8*)cpInfo[attributeNameIndex];
    std::cout << "Attribute name:  " << attributeName->bytes << std::endl;
    uint32_t attributeLength = readU4(classCursor);
    if (attributeName->bytes == ConstantValueAttribute::attributeName) {
      ConstantValueAttribute* constantValueAttribute =
          new ConstantValueAttribute();
      constantValueAttribute->constantValueIndex = readU2(classCursor);
      ai = constantValueAttribute;
    } else if (attributeName->bytes == CodeAttribute::attributeName) {
      CodeAttribute* codeAttribute = new CodeAttribute();
      codeAttribute->maxStack = readU2(classCursor);
      codeAttribute->maxLocals = readU2(classCursor);
      uint32_t codeLength = readU4(classCursor);
      codeAttribute->code = std::vector(classCursor, classCursor + codeLength);
      advance(classCursor, codeLength);
      uint16_t exceptionTableLength = readU2(classCursor);
      std::vector<ExceptionEntry> exceptionTable;
      if (exceptionTableLength != 0) {
        std::cout << "Exceptions needs to be parsed!";
        throw;
      }
      for (int i = 0; i < exceptionTableLength; i++) {
        // TODO: Parse exception
      }
      codeAttribute->exceptionTable = exceptionTable;
      codeAttribute->attributes = parseAttributeInfo(classCursor, cpInfo);
      ai = codeAttribute;
    } else if (attributeName->bytes == StackMapTableAttribute::attributeName) {
      StackMapTableAttribute* stackMapTableAttribute =
          new StackMapTableAttribute();
      uint16_t numberOfEntries = readU2(classCursor);
      std::vector<StackMapFrame> entries;
      for (int i = 0; i < numberOfEntries; i++) {
        uint8_t tag = readU1(classCursor);
        StackMapFrame frame;
        if (tag < 64) {
          frame.type = Same;
        } else if (tag < 128) {
          frame.type = SameLocals;
          frame.stack = parseVerificationInfo(classCursor, 1);
        } else if (tag == 247) {
          frame.type = SameLocalsExtended;
          frame.stack = parseVerificationInfo(classCursor, 1);
        } else if (tag == 248 || tag == 249 || tag == 250) {
          frame.type = Chop;
          frame.offsetDelta = readU2(classCursor);
        } else if (tag == 251) {
          frame.type = SameExtended;
          frame.offsetDelta = readU2(classCursor);
        } else if (tag == 252 || tag == 253 || tag == 254) {
          frame.type = Append;
          frame.offsetDelta = readU2(classCursor);
          frame.locals = parseVerificationInfo(classCursor, tag - 251);
        }

        else if (tag == 255) {
          frame.type = Full;
          frame.offsetDelta = readU2(classCursor);
          frame.locals =
              parseVerificationInfo(classCursor, readU2(classCursor));
          frame.stack = parseVerificationInfo(classCursor, readU2(classCursor));
        } else {  // Sanity check
          std::cout << "StackMapAttribute tag: " << (size_t)tag << " not caught"
                    << std::endl;
          throw;
        }
        entries.push_back(frame);
      }
      stackMapTableAttribute->entries = entries;
      ai = stackMapTableAttribute;
    } else if (attributeName->bytes ==
               LineNumberTableAttribute::attributeName) {
      LineNumberTableAttribute* lineNumberTableAttribute =
          new LineNumberTableAttribute();
      uint16_t lineNumberTableLength = readU2(classCursor);
      std::vector<LineNumberEntry> lineNumberTable;
      for (int i = 0; i < lineNumberTableLength; i++) {
        LineNumberEntry entry;
        entry.startPC = readU2(classCursor);
        entry.lineNumber = readU2(classCursor);
        lineNumberTable.push_back(entry);
      }
      lineNumberTableAttribute->lineNumberTable = lineNumberTable;
      ai = lineNumberTableAttribute;

      //  else if (attributeName->bytes ==
      //            LocalVariableTableAttribute::attributeName){
      //   // Parse LocalVariableTableAttribute
      // } else if (attributeName->bytes ==
      //            LocalVariableTypeTableAttribute::attributeName){
      //   // Parse LocalVariableTypeTableAttribute
    } else if (attributeName->bytes == SourceFileAttribute::attributeName) {
      SourceFileAttribute* sourceFileAttribute = new SourceFileAttribute();
      sourceFileAttribute->sourceFileIndex = readU2(classCursor);
      ai = sourceFileAttribute;
    } else {
      std::cout << "-> Cursor jump: " << attributeLength << " ->" << std::endl;
      advance(classCursor, attributeLength);
    }
    if (ai != NULL) {
      // To check that we don't need a list in the values of the map instead of
      // just one object
      auto finder = attributes.find(attributeName->bytes);
      assert(finder == attributes.end());
      attributes[attributeName->bytes] = ai;
    }
  }
  return attributes;
}

std::vector<VerificationInfo> Parser::parseVerificationInfo(
    std::vector<uint8_t>::iterator& classCursor, uint16_t entries) {
  std::vector<VerificationInfo> verifications;
  for (int i = 0; i < entries; i++) {
    VerificationInfo info;
    info.tag = static_cast<VerificationType>(readU1(classCursor));
    if (info.tag == Object || info.tag == Uninitialized) {
      info.cPoolIndexOrOffset = readU2(classCursor);
    }
    verifications.push_back(info);
  }
  return verifications;
}

uint8_t Parser::readU1(std::vector<uint8_t>::iterator& cursor) {
  return *(cursor++);
}

uint16_t Parser::readU2(std::vector<uint8_t>::iterator& cursor) {
  uint8_t highByte = readU1(cursor), lowByte = readU1(cursor);
  return ((uint16_t)highByte << 8) | lowByte;
}

uint32_t Parser::readU4(std::vector<uint8_t>::iterator& cursor) {
  uint32_t highTwoByte = readU2(cursor), lowTwoByte = readU2(cursor);
  return ((uint32_t)highTwoByte << 16) | lowTwoByte;
}