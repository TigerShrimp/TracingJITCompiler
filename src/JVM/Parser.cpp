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

  //   ClassFile {
  //     u4             magic;
  uint32_t magic = readU4(classCursor);
  std::cout << "Magic: " << std::hex << magic << std::dec << std::endl;
  //     u2             minor_version;
  uint16_t minorVersion = readU2(classCursor);
  std::cout << "MinorVersion: " << minorVersion << std::endl;
  //     u2             major_version;
  uint16_t majorVersion = readU2(classCursor);
  std::cout << "MajorVersion: " << majorVersion << std::endl;
  //     u2             constant_pool_count;
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
        constantUtf8->length = readU2(classCursor);
        std::string str(classCursor, classCursor + constantUtf8->length);
        assert(str.length() == constantUtf8->length);
        advance(classCursor, constantUtf8->length);
        constantUtf8->bytes = str;
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

  //     cp_info        constant_pool[constant_pool_count-1];
  std::cout << "CPInfo: " << constantPoolCount << std::endl;
  for (auto cp : cpInfo) {
    std::cout << "\t" << cp->info() << std::endl;
  }
  //     u2             access_flags;
  uint16_t accessFlags = readU2(classCursor);
  std::cout << "AccessFlags: " << accessFlags << std::endl;
  //     u2             this_class;
  uint16_t thisClass = readU2(classCursor);
  std::cout << "ThisClass: " << thisClass << std::endl;
  //     u2             super_class;
  uint16_t superClass = readU2(classCursor);

  std::cout << "SuperClass: " << superClass << std::endl;

  //     u2             interfaces_count;
  uint16_t interfacesCount = readU2(classCursor);
  std::vector<uint16_t> interfaces;
  for (int i = 0; i < interfacesCount; i++) {
    //     u2             interfaces[interfaces_count];s
    interfaces.push_back(readU2(classCursor));
  }
  std::cout << "Interfaces: " << interfacesCount << std::endl;
  for (auto interface : interfaces) {
    std::cout << "\t" << interface << std::endl;
  }

  //     u2             fields_count;
  uint16_t fieldsCount = readU2(classCursor);
  //     field_info     fields[fields_count];
  std::cout << "Fields: " << fieldsCount << std::endl;
  std::vector<FieldInfo> fields;
  for (int i = 0; i < fieldsCount; i++) {
    FieldInfo fieldInfo;
    fieldInfo.accsessFlag = readU2(classCursor);
    fieldInfo.nameIndex = readU2(classCursor);
    fieldInfo.descriptorIndex = readU2(classCursor);
    uint16_t attributesCount = readU2(classCursor);
    fieldInfo.attributes =
        parseAttributeInfo(classCursor, attributesCount, cpInfo);
    fields.push_back(fieldInfo);
  }

  for (auto field : fields) {
    std::cout << "\t" << cpInfo[field.nameIndex]->info() << " "
              << cpInfo[field.descriptorIndex]->info() << std::endl;
  }
  //     u2             methods_count;
  uint16_t methodsCount = readU2(classCursor);
  //     method_info    methods[methods_count];
  std::cout << "Methods: " << methodsCount << std::endl;
  std::vector<MethodInfo> methods;
  for (int i = 0; i < methodsCount; i++) {
    MethodInfo methodInfo;
    methodInfo.accsessFlag = readU2(classCursor);
    methodInfo.nameIndex = readU2(classCursor);
    methodInfo.descriptorIndex = readU2(classCursor);
    uint16_t attributesCount = readU2(classCursor);
    methodInfo.attributes =
        parseAttributeInfo(classCursor, attributesCount, cpInfo);
    methods.push_back(methodInfo);
    std::cout << "\t" << cpInfo[methodInfo.nameIndex]->info() << " "
              << cpInfo[methodInfo.descriptorIndex]->info() << std::endl;
  }
  //     u2             attributes_count;
  //     attribute_info attributes[attributes_count];
  // }

  std::cout << "\n---- End of Parse ----\n" << std::endl;
  return {};
}

std::vector<AttributeInfo*> Parser::parseAttributeInfo(
    std::vector<uint8_t>::iterator& classCursor, const uint16_t attributesCount,
    std::vector<CPInfo*>& cpInfo) {
  std::vector<AttributeInfo*> attributes;
  std::cout << "ParseAttributeInfo: " << attributesCount << std::endl;
  for (int i = 0; i < attributesCount; i++) {
    uint16_t attributeNameIndex = readU2(classCursor);
    std::cout << "name index: " << attributeNameIndex << std::endl;
    ConstantUtf8* attributeName = (ConstantUtf8*)cpInfo[attributeNameIndex];
    std::cout << "Attribute name:(" << i << "/" << attributesCount
              << "): " << attributeName->bytes << std::endl;
    uint32_t attributeLength = readU4(classCursor);
    /*if (attributeName->bytes == ConstantValueAttribute::attributeName) {
      // Parse ConstantValueAttribute
    } else*/
    if (attributeName->bytes == CodeAttribute::attributeName) {
      // Parse CodeAttribute
      CodeAttribute* codeAttribute = new CodeAttribute();
      codeAttribute->maxStack = readU2(classCursor);
      std::cout << "Max Stack: " << codeAttribute->maxStack << std::endl;
      codeAttribute->maxLocals = readU2(classCursor);
      std::cout << "Max Locals: " << codeAttribute->maxLocals << std::endl;
      uint32_t codeLength = readU4(classCursor);
      std::cout << "Code length: " << codeLength << std::endl;
      std::vector<uint8_t> code(classCursor, classCursor + codeLength);
      codeAttribute->code = code;
      for (auto byte : code) {
        std::cout << std::hex << "0x" << (size_t)byte << std::dec << std::endl;
      }
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
      uint16_t internalAttributesCount = readU2(classCursor);
      codeAttribute->attributes =
          parseAttributeInfo(classCursor, internalAttributesCount, cpInfo);
      attributes.push_back(codeAttribute);
      // } else if (attributeName->bytes ==
      //            StackMapTableAttribute::attributeName){
      //   // Parse StackMapTableAttribute
    } else if (attributeName->bytes ==
               LineNumberTableAttribute::attributeName) {
      // Parse LineNumberTableAttribute
      LineNumberTableAttribute* lineNumberTableAttribute =
          new LineNumberTableAttribute();
      uint16_t lineNumberTableLength = readU2(classCursor);
      std::vector<LineNumberEntry> lineNumberTable;
      assert(2 + lineNumberTableLength * 4 == attributeLength);
      for (int i = 0; i < lineNumberTableLength; i++) {
        LineNumberEntry entry;
        entry.startPC = readU2(classCursor);
        entry.lineNumber = readU2(classCursor);
        lineNumberTable.push_back(entry);
      }
      lineNumberTableAttribute->lineNumberTable = lineNumberTable;
      attributes.push_back(lineNumberTableAttribute);

      //  else if (attributeName->bytes ==
      //            LocalVariableTableAttribute::attributeName){
      //   // Parse LocalVariableTableAttribute
      // } else if (attributeName->bytes ==
      //            LocalVariableTypeTableAttribute::attributeName){
      //   // Parse LocalVariableTypeTableAttribute
    } else {
      std::cout << "-> Cursor jump: " << attributeLength << " ->" << std::endl;
      advance(classCursor, attributeLength);
    }
  }
  return attributes;
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