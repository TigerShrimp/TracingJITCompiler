#include "JVM/ClassFile.hpp"
using namespace std;

string ConstantClass::info() { return "ConstantClass: "; }
string ConstantFieldRef::info() { return "ConstantFieldRef: "; }
string ConstantMethodRef::info() {
  return "ConstantMethodRef: " + to_string(nameAndTypeIndex);
}
string ConstantInterfaceMethodRef::info() {
  return "ConstantInterfaceMethodRef: ";
}
string ConstantString::info() { return "ConstantString: "; }
string ConstantInteger::info() { return "ConstantInteger: "; }
string ConstantFloat::info() { return "ConstantFloat: "; }
string ConstantLong::info() { return "ConstantLong: "; }
string ConstantDouble::info() { return "ConstantDouble: "; }
string ConstantNameAndType::info() {
  return "ConstantNameAndType: " + to_string(nameIndex);
}
string ConstantUtf8::info() { return "ConstantUtf8: " + bytes; }
string ConstantMethodHandle::info() { return "ConstantMethodHandle: "; }
string ConstantMethodType::info() { return "ConstantMethodType: "; }
string ConstantInvokeDynamic::info() { return "ConstantInvokeDynamic: "; }

const string ConstantValueAttribute::attributeName = "ConstantValue";
const string CodeAttribute::attributeName = "Code";
const string StackMapTableAttribute::attributeName = "StackMapTable";
const string SourceFileAttribute::attributeName = "SourceFile";
const string BootstrapMethodsAttribute::attributeName = "BootstrapMethods";
const string NestHostAttribute::attributeName = "NestHost";
const string NestMembersAttribute::attributeName = "NestMembers";

string ClassFile::contentsString() {
  stringstream ss;
  SourceFileAttribute* sfa =
      (SourceFileAttribute*)attributes[SourceFileAttribute::attributeName];
  ConstantUtf8* fileName = (ConstantUtf8*)constantPool[sfa->sourceFileIndex];
  ss << "\n----- Class File (source: " << fileName->bytes
     << ") contents: -----\n"
     << endl;
  ss << "Magic: " << hex << magic << dec << endl;
  ss << "MinorVersion: " << minorVersion << endl;
  ss << "MajorVersion: " << majorVersion << endl;
  ss << "Constant Pool Info: " << constantPool.size() << endl;
  for (auto cpInfo : constantPool) {
    ss << " " << cpInfo.second->info() << endl;
  }
  ss << "AccessFlags: " << accessFlags << endl;
  ss << "ThisClass: " << thisClass << endl;
  ss << "SuperClass: " << superClass << endl;
  ss << "Interfaces: " << interfaces.size() << endl;
  for (auto interface : interfaces) {
    ss << " " << interface << endl;
  }
  ss << "Fields: " << fields.size() << endl;
  for (auto field : fields) {
    ss << " " << constantPool[field.nameIndex]->info() << " - "
       << constantPool[field.descriptorIndex]->info() << endl;
  }
  ss << "Methods: " << methods.size() << endl;
  for (auto method : methods) {
    ss << " " << constantPool[method.nameIndex]->info() << " - "
       << constantPool[method.descriptorIndex]->info() << endl;
    CodeAttribute* ca =
        (CodeAttribute*)method.attributes[CodeAttribute::attributeName];

    ss << "  Max Stack:" << ca->maxStack << endl;
    ss << "  Max Locals:" << ca->maxLocals << endl;
    ss << "  Attributes: " << ca->attributes.size() << endl;
    ss << "  Code:" << endl;
    ss << hex;
    for (auto byte : ca->code) {
      ss << "   0x" << (size_t)byte << endl;
    }
    ss << dec;
  }
  ss << "Attributes: " << attributes.size() << endl;
  ss << "\n----- End of Class File -----\n" << endl;
  return ss.str();
}