#include "JVM/ClassFile.hpp"
using namespace std;

string ConstantClass::info() { return "ConstantClass: "; }
string ConstantFieldRef::info() { return "ConstantFieldRef: "; }
string ConstantMethodRef::info() { return "ConstantMethodRef: "; }
string ConstantInterfaceMethodRef::info() {
  return "ConstantInterfaceMethodRef: ";
}
string ConstantString::info() { return "ConstantString: "; }
string ConstantInteger::info() { return "ConstantInteger: "; }
string ConstantFloat::info() { return "ConstantFloat: "; }
string ConstantLong::info() { return "ConstantLong: "; }
string ConstantDouble::info() { return "ConstantDouble: "; }
string ConstantNameAndType::info() { return "ConstantNameAndType: "; }
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

void ClassFile::printContents() {
  SourceFileAttribute* sfa =
      (SourceFileAttribute*)attributes[SourceFileAttribute::attributeName];
  ConstantUtf8* fileName = (ConstantUtf8*)constantPool[sfa->sourceFileIndex];
  cout << "\n----- Class File (source: " << fileName->bytes
       << ") contents: -----\n"
       << endl;
  cout << "Magic: " << hex << magic << dec << endl;
  cout << "MinorVersion: " << minorVersion << endl;
  cout << "MajorVersion: " << majorVersion << endl;
  cout << "Constant Pool Info: " << constantPool.size() << endl;
  for (auto cpInfo : constantPool) {
    cout << " " << cpInfo->info() << endl;
  }
  cout << "AccessFlags: " << accessFlags << endl;
  cout << "ThisClass: " << thisClass << endl;
  cout << "SuperClass: " << superClass << endl;
  cout << "Interfaces: " << interfaces.size() << endl;
  for (auto interface : interfaces) {
    cout << " " << interface << endl;
  }
  cout << "Fields: " << fields.size() << endl;
  for (auto field : fields) {
    cout << " " << constantPool[field.nameIndex]->info() << " - "
         << constantPool[field.descriptorIndex]->info() << endl;
  }
  cout << "Methods: " << methods.size() << endl;
  for (auto method : methods) {
    cout << " " << constantPool[method.nameIndex]->info() << " - "
         << constantPool[method.descriptorIndex]->info() << endl;
    CodeAttribute* ca =
        (CodeAttribute*)method.attributes[CodeAttribute::attributeName];

    cout << "  Max Stack:" << ca->maxStack << endl;
    cout << "  Max Locals:" << ca->maxLocals << endl;
    cout << "  Attributes: " << ca->attributes.size() << endl;
    cout << "  Code:" << endl;
    cout << hex;
    for (auto byte : ca->code) {
      cout << "   0x" << (size_t)byte << endl;
    }
    cout << dec;
  }
  cout << "Attributes: " << attributes.size() << endl;
  cout << "\n----- End of Class File -----\n" << endl;
}