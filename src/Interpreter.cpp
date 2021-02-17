#include "Interpreter.hpp"

using namespace std;
Interpreter::Interpreter(Program prg) : pc(0), program(prg), states(){};

void Interpreter::interpret() {
  uint16_t main = findIndexOfMain();
  invoke(main);
}

uint16_t Interpreter::findIndexOfMain() {
  for (auto method : program.methods) {
    uint16_t nameIndex = method.first;
    std::string constantString =
        ((ConstantUtf8 *)program.constantPool[nameIndex])->bytes;
    if (constantString == "main") {
      return nameIndex;
    }
  }
  // TODO: Standardize execption handing.
  std::cerr << "Class file containsn't a main function" << std::endl;
  throw;
}

void Interpreter::eval() {
  State *state = states.top();
  vector<uint8_t> code = program.methods[state->method].code;
  while (true) {
    Mnemonic mnemonic =
        static_cast<Mnemonic>(program.methods[state->method].code[state->pc]);
    cout << byteCodeNames.at(mnemonic) << ": " << state->pc << endl;
    switch (mnemonic) {
      // Constants loading
      case ICONST_0: {
        push({.intValue = 0});
        break;
      }
      case ICONST_1: {
        push({.intValue = 1});
        break;
      }
      case SIPUSH: {
        int param = getParametersAsInt();
        push({.intValue = param});
        state->pc += 2;
        break;
      }
      // Loading
      case ILOAD_1: {
        load(1);
        break;
      }
      case ILOAD_2: {
        load(2);
        break;
      }
      case ILOAD_3: {
        load(3);
        break;
      }
      // Storing
      case ISTORE_1: {
        store(1);
        break;
      }
      case ISTORE_2: {
        store(2);
        break;
      }
      case ISTORE_3: {
        store(3);
        break;
      }
      // Control flow
      case IF_ICMPGE: {
        int rhs = pop().intValue;
        int lhs = pop().intValue;
        int offset = getParametersAsInt();
        if (lhs >= rhs) {
          state->pc += (offset - 1);
        } else {
          state->pc += 2;
        }
        break;
      }
      case GOTO: {
        int offset = getParametersAsInt();
        state->pc += (offset - 1);
        break;
      }
      case RETURN: {
        State *state = states.top();
        states.pop();
        delete state;
        return;
      }
      case INVOKEVIRTUAL: {
        cout << pop().intValue << endl;
        state->pc += 2;
        break;
      }
      // Arithmetic
      case IADD: {
        int rhs = pop().intValue;
        int lhs = pop().intValue;
        int res = lhs + rhs;
        push({.intValue = res});
        break;
      }
      case IINC: {
        uint8_t index = code[state->pc + 1];
        uint8_t constant = code[state->pc + 2];
        int value = state->locals[index].intValue;
        value += constant;
        state->locals[index].intValue = value;
        state->pc += 2;
        break;
      }
      // Misc
      case GETSTATIC: {
        // int index = getParametersAsInt();
        // ConstantFieldRef* value =
        // (ConstantFieldRef*)program.constantPool[index]; value->classIndex;
        state->pc += 2;
        break;
      }
      case NOP:
        break;
      default: {
        cout << "byte-code " << byteCodeNames.at(mnemonic)
             << " not supported at this moment, please come back later <3"
             << endl;
        break;
      }
    }
    state->pc += 1;
  }
}

void Interpreter::invoke(uint16_t nameIndex) {
  Method methodToInvoke = program.methods[nameIndex];
  stack<Value> stack;
  map<size_t, Value> locals;
  if (!states.empty()) {
    vector<Type> argTypes = methodToInvoke.argTypes;
    size_t key = 0;
    for (Type type : argTypes) {
      Value toAdd = pop();
      locals[key] = toAdd;
      key += sizeOf(type);
    }
  }
  State *state = new State{0, nameIndex, stack, locals};
  states.push(state);
  eval();
}

void Interpreter::load(size_t var) {
  Value val = states.top()->locals[var];
  push(val);
}

void Interpreter::store(size_t var) {
  Value val = pop();
  states.top()->locals[var] = val;
}

// Helper functions
Value Interpreter::pop() {
  Value val = states.top()->stack.top();
  states.top()->stack.pop();
  return val;
}

void Interpreter::push(Value value) { states.top()->stack.push(value); }

int Interpreter::getParametersAsInt() {
  State *state = states.top();
  vector<uint8_t> code = program.methods[state->method].code;
  uint8_t byte1 = code[state->pc + 1];
  uint8_t byte2 = code[state->pc + 2];
  short res = byte1 << 8 | byte2;
  return (int)res;
}

size_t Interpreter::sizeOf(Type type) {
  switch (type.type) {
    case Int:
    case Float:
      return 1;
    case Long:
    case Double:
      return 2;
    default:
      return 0;
  }
}