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

  //     u2             minor_version;
  uint16_t minorVersion = readU2(classCursor);
  //     u2             major_version;
  uint16_t majorVersion = readU2(classCursor);

  //     u2             constant_pool_count;
  uint16_t constantPoolCount = readU2(classCursor);

  std::vector<CPInfo*> cpInfo;
  for (uint16_t i = 0; i < constantPoolCount - 1; i++) {
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
        classCursor += constantUtf8->length;
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
    }
  }

  //     cp_info        constant_pool[constant_pool_count-1];

  //     u2             access_flags;
  uint16_t accessFlags = readU2(classCursor);

  //     u2             this_class;
  uint16_t thisClass = readU2(classCursor);

  //     u2             super_class;
  uint16_t superClass = readU2(classCursor);

  //     u2             interfaces_count;
  uint16_t interfacesCount = readU2(classCursor);
  std::vector<uint16_t> interfaces;
  for (int i = 0; i < interfacesCount; i++) {
    //     u2             interfaces[interfaces_count];s
    interfaces.push_back(readU2(classCursor));
  }

  //     u2             fields_count;
  uint16_t fieldsCount = readU2(classCursor);
  //     field_info     fields[fields_count];
  //     u2             methods_count;
  //     method_info    methods[methods_count];
  //     u2             attributes_count;
  //     attribute_info attributes[attributes_count];
  // }

  std::cout << "Magic: " << magic << std::endl;
  std::cout << "MinorVersion: " << minorVersion << std::endl;
  std::cout << "MajorVersion: " << majorVersion << std::endl;
  std::cout << "CPInfo: " << constantPoolCount << std::endl;
  for (auto cp : cpInfo) {
    std::cout << "\t" << cp->info() << std::endl;
  }
  std::cout << "AccessFlags: " << accessFlags << std::endl;
  std::cout << "ThisClass: " << thisClass << std::endl;
  std::cout << "SuperClass: " << superClass << std::endl;
  std::cout << "Interfaces: " << interfacesCount << std::endl;
  for (auto interface : interfaces) {
    std::cout << "\t" << interface << std::endl;
  }
  std::cout << "FieldsCount: " << fieldsCount << std::endl;
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