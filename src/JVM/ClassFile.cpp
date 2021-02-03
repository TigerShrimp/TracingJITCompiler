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