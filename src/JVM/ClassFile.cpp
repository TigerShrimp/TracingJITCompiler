#include "JVM/ClassFile.hpp"

std::string ConstantClass::info() { return "ConstantClass: "; }
std::string ConstantFieldRef::info() { return "ConstantFieldRef: "; }
std::string ConstantMethodRef::info() { return "ConstantMethodRef: "; }
std::string ConstantInterfaceMethodRef::info() {
  return "ConstantInterfaceMethodRef: ";
}
std::string ConstantString::info() { return "ConstantString: "; }
std::string ConstantInteger::info() { return "ConstantInteger: "; }
std::string ConstantFloat::info() { return "ConstantFloat: "; }
std::string ConstantLong::info() { return "ConstantLong: "; }
std::string ConstantDouble::info() { return "ConstantDouble: "; }
std::string ConstantNameAndType::info() { return "ConstantNameAndType: "; }
std::string ConstantUtf8::info() { return "ConstantUtf8: " + bytes; }
std::string ConstantMethodHandle::info() { return "ConstantMethodHandle: "; }
std::string ConstantMethodType::info() { return "ConstantMethodType: "; }
std::string ConstantInvokeDynamic::info() { return "ConstantInvokeDynamic: "; }

const std::string ConstantValueAttribute::attributeName = "ConstantValue";
const std::string CodeAttribute::attributeName = "Code";
const std::string StackMapTableAttribute::attributeName = "StackMapTable";
const std::string LineNumberTableAttribute::attributeName = "LineNumberTable";
const std::string LocalVariableTableAttribute::attributeName =
    "LocalVariableTable";
const std::string LocalVariableTypeTableAttribute::attributeName =
    "LocalVariableTypeTable";

void ClassFile::printContents() {
  std::cout << "\n----- Class File contents: -----\n" << std::endl;
  std::cout << "Magic: " << magic << std::endl;
  std::cout << "MinorVersion: " << minorVersion << std::endl;
  std::cout << "MajorVersion: " << majorVersion << std::endl;
  std::cout << "Constant Pool Info: " << constantPool.size() << std::endl;
  for (auto cpInfo : constantPool) {
    std::cout << "\t" << cpInfo->info() << std::endl;
  }
  std::cout << "AccessFlags: " << accessFlags << std::endl;
  std::cout << "ThisClass: " << thisClass << std::endl;
  std::cout << "SuperClass: " << superClass << std::endl;
  std::cout << "Interfaces: " << interfaces.size() << std::endl;
  for (auto interface : interfaces) {
    std::cout << "\t" << interface << std::endl;
  }
  std::cout << "Fields: " << fields.size() << std::endl;
  for (auto field : fields) {
    std::cout << "\t" << constantPool[field.nameIndex]->info() << " - "
              << constantPool[field.nameIndex]->info() << std::endl;
  }
  std::cout << "Methods: " << methods.size() << std::endl;
  for (auto method : methods) {
    std::cout << "\t" << constantPool[method.nameIndex]->info() << " - "
              << constantPool[method.nameIndex]->info() << std::endl;
  }
  std::cout << "Attributes: " << attributes.size() << std::endl;
  std::cout << "\n----- End of Class File -----\n" << std::endl;
}