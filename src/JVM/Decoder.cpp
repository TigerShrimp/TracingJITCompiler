#include "JVM/Decoder.hpp"

using namespace std;

Program* Decoder::decode(ClassFile classFile) {
  Program* prg = new Program();
  prg->constantPool = classFile.constantPool;
  map<uint16_t, Method> methods;
  for (MethodInfo methodInfo : classFile.methods) {
    Method method = {methodInfo.nameIndex};
    parseTypes(classFile.constantPool[methodInfo.descriptorIndex], method);
    CodeAttribute* code =
        (CodeAttribute*)methodInfo.attributes[CodeAttribute::attributeName];
    method.maxStack = code->maxStack;
    method.maxLocals = code->maxLocals;
    method.code = code->code;

    if (methodInfo.attributes.find(ConstantValueAttribute::attributeName) !=
        methodInfo.attributes.end()) {
      method.constant =
          *(ConstantValueAttribute*)
               methodInfo.attributes[ConstantValueAttribute::attributeName];
    }
    if (methodInfo.attributes.find(StackMapTableAttribute::attributeName) !=
        methodInfo.attributes.end()) {
      method.stackMapTable =
          *(StackMapTableAttribute*)
               methodInfo.attributes[StackMapTableAttribute::attributeName];
    }
    methods[methodInfo.nameIndex] = method;
  }

  prg->methods = methods;
  return prg;
}

void Decoder::parseTypes(CPInfo* descriptor, Method& method) {
  ConstantUtf8* descriptorString = (ConstantUtf8*)descriptor;
  //   cout << descriptorString->bytes << endl;
  smatch matches;
  if (regex_search(descriptorString->bytes, matches, typesRegex)) {
    string argString = matches[1];
    string retString = matches[2];
    vector<Type> types;
    while (argString.length() > 0) {
      Type type = getTypeFromString(argString);
      types.push_back(type);
      size_t length = getTypeStringLength(type);
      argString = argString.substr(length, argString.length() - length);
    }
    method.argTypes = types;
    method.retType = getTypeFromString(retString);
  } else {
    cerr << "Ånej regex" << endl;
    throw;
  }
}

size_t Decoder::getTypeStringLength(Type type) {
  switch (type.type) {
    case String:
      return 18;
    case List:
      return 1 + getTypeStringLength(*(type.subType));
    default:
      return 1;
  }
}

Type Decoder::getTypeFromString(string typeString) {
  switch (typeString[0]) {
    case 'I':
      return {Int};
    case 'F':
      return {Float};
    case 'D':
      return {Double};
    case 'V':
      return {Void};
    case 'J':
      return {Long};
    case '[': {
      Type* subType = new Type();
      Type st =
          getTypeFromString(typeString.substr(1, typeString.length() - 1));
      subType->type = st.type;
      subType->subType = st.subType;
      return {List, subType};
    }
    default:
      return {String};
  }
}