#include "Interpreter.hpp"

using namespace std;

Interpreter::Interpreter(Program prg) : pc(0), program(prg), states(){};

void Interpreter::interpret() {
  uint16_t main = findIndexOfMain();
  invoke(main);
}

size_t Interpreter::findIndexOfMain() {
  for (auto method : program.methods) {
    uint16_t nameIndex = method.first;
    string constantString =
        ((ConstantUtf8 *)program.constantPool[nameIndex])->bytes;
    if (constantString == "main") {
      return nameIndex;
    }
  }
  // TODO: Standardize execption handing.
  cerr << "Class file containsn't a main function" << endl;
  throw;
}

void Interpreter::eval() {
  State *state = states.top();
  vector<uint8_t> code = program.methods[state->method].code;
  while (true) {
    Mnemonic mnemonic =
        static_cast<Mnemonic>(program.methods[state->method].code[state->pc]);
    switch (mnemonic) {
      // Constants loading
      case ICONST_M1: {
        push(Value(-1));
        break;
      }
      case ICONST_0: {
        push(Value(0));
        break;
      }
      case ICONST_1: {
        push(Value(1));
        break;
      }
      case ICONST_2: {
        push(Value(2));
        break;
      }
      case ICONST_3: {
        push(Value(3));
        break;
      }
      case ICONST_4: {
        push(Value(4));
        break;
      }
      case ICONST_5: {
        push(Value(5));
        break;
      }
      case LCONST_0: {
        push(Value(0L));
        break;
      }
      case LCONST_1: {
        push(Value(1L));
        break;
      }
      case FCONST_0: {
        push(Value(0.0f));
        break;
      }
      case FCONST_1: {
        push(Value(1.0f));
        break;
      }
      case FCONST_2: {
        push(Value(2.0f));
        break;
      }
      case DCONST_0: {
        push(Value(0.0));
        break;
      }
      case DCONST_1: {
        push(Value(1.0));
        break;
      }
      case BIPUSH: {
        uint8_t byte = code[state->pc + 1];
        int8_t sByte = (int8_t)byte;
        push(Value(sByte));
        state->pc += 1;
        break;
      }
      case SIPUSH: {
        int param = getParametersAsInt();
        push(Value(param));
        state->pc += 2;
        break;
      }
      // Loading
      case LDC: {
        int index = code[state->pc + 1];
        CPInfo *cpInfoEntry = program.constantPool[index];
        if (cpInfoEntry->info() == "ConstantFloat: ") {
          ConstantFloat *constantFloat = (ConstantFloat *)cpInfoEntry;
          float floatRes;
          memcpy(&floatRes, &constantFloat->bytes, sizeof(float));
          push(Value(floatRes));
        } else {  // ConstantInt
          ConstantInteger *constantInteger = (ConstantInteger *)cpInfoEntry;
          int intRes = (int)constantInteger->bytes;
          push(Value(intRes));
        }
        state->pc += 1;
        break;
      }
      case LDC2_W: {
        // TODO: Make Long/Double constants more general
        int index = getParametersAsInt();
        CPInfo *cpInfoEntry = program.constantPool[index];
        if (cpInfoEntry->info() == "ConstantDouble: ") {
          ConstantDouble *constantDouble = (ConstantDouble *)cpInfoEntry;
          // The high and low bytes are stored in different positions of the
          // constant pool so they have to be combined into one 64-bit number
          long longRes = ((long)constantDouble->highBytes << 32) +
                         constantDouble->lowBytes;
          double doubleRes;
          memcpy(&doubleRes, &longRes, sizeof(double));
          push(Value(doubleRes));
        } else {  // ConstantLong
          ConstantLong *constantLong = (ConstantLong *)cpInfoEntry;
          long longRes =
              ((long)constantLong->highBytes << 32) + constantLong->lowBytes;
          push(Value(longRes));
        }
        state->pc += 2;
        break;
      }
      case ILOAD:
      case LLOAD:
      case FLOAD:
      case DLOAD: {
        int index = code[state->pc + 1];
        load(index);
        state->pc += 1;
        break;
      }
      case ILOAD_0:
      case LLOAD_0:
      case FLOAD_0:
      case DLOAD_0: {
        load(0);
        break;
      }
      case ILOAD_1:
      case LLOAD_1:
      case FLOAD_1:
      case DLOAD_1: {
        load(1);
        break;
      }
      case ILOAD_2:
      case LLOAD_2:
      case FLOAD_2:
      case DLOAD_2: {
        load(2);
        break;
      }
      case ILOAD_3:
      case LLOAD_3:
      case FLOAD_3:
      case DLOAD_3: {
        load(3);
        break;
      }
      // Storing
      case ISTORE:
      case LSTORE:
      case FSTORE:
      case DSTORE: {
        int index = code[state->pc + 1];
        store(index);
        state->pc += 1;
        break;
      }
      case ISTORE_0:
      case LSTORE_0:
      case FSTORE_0:
      case DSTORE_0: {
        store(0);
        break;
      }
      case ISTORE_1:
      case LSTORE_1:
      case FSTORE_1:
      case DSTORE_1: {
        store(1);
        break;
      }
      case ISTORE_2:
      case LSTORE_2:
      case FSTORE_2:
      case DSTORE_2: {
        store(2);
        break;
      }
      case ISTORE_3:
      case LSTORE_3:
      case FSTORE_3:
      case DSTORE_3: {
        store(3);
        break;
      }
      // Comparisons
      case LCMP:
      case FCMPL:
      case FCMPG:
      case DCMPL:
      case DCMPG: {
        Value rhs = pop();
        Value lhs = pop();
        if (lhs < rhs)
          push(Value(-1));
        else if (rhs < lhs)
          push(Value(1));
        else
          push(Value(0));
        break;
      }
      // Control flow
      case IFGT: {
        int value = pop().val.intValue;
        int offset = getParametersAsInt();
        state->pc += (value > 0) ? (offset - 1) : 2;
        break;
      }
      case IF_ICMPGE: {
        int rhs = pop().val.intValue;
        int lhs = pop().val.intValue;
        int offset = getParametersAsInt();
        state->pc += (lhs >= rhs) ? (offset - 1) : 2;
        break;
      }
      case IF_ICMPGT: {
        int rhs = pop().val.intValue;
        int lhs = pop().val.intValue;
        int offset = getParametersAsInt();
        state->pc += (lhs > rhs) ? (offset - 1) : 2;
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
      case IRETURN:
      case LRETURN:
      case FRETURN:
      case DRETURN: {
        State *state = states.top();
        states.pop();
        Value returnValue = state->stack.top();
        delete state;
        states.top()->stack.push(returnValue);
        return;
      }
      case INVOKEVIRTUAL: {
        // Is currently only used when System.out.println is called
        Value val = pop();
        cout << setprecision(17) << toString(val) << endl;
        state->pc += 2;
        break;
      }
      case INVOKESTATIC: {
        int methodRefIndex = getParametersAsInt();
        size_t nameIndex = findNameIndex(methodRefIndex);
        invoke(nameIndex);
        state->pc += 2;
        break;
      }
      // Arithmetic
      case IADD:
      case LADD:
      case FADD:
      case DADD: {
        Value rhs = pop();
        Value lhs = pop();
        push(lhs + rhs);
        break;
      }
      case ISUB:
      case LSUB:
      case FSUB:
      case DSUB: {
        Value rhs = pop();
        Value lhs = pop();
        push(lhs - rhs);
        break;
      }
      case IMUL:
      case LMUL:
      case FMUL:
      case DMUL: {
        Value rhs = pop();
        Value lhs = pop();
        push(lhs * rhs);
        break;
      }
      case IDIV:
      case LDIV:
      case FDIV:
      case DDIV: {
        Value rhs = pop();
        Value lhs = pop();
        push(lhs / rhs);
        break;
      }
      case IINC: {
        uint8_t index = code[state->pc + 1];
        uint8_t constant = code[state->pc + 2];
        int value = state->locals[index].val.intValue;
        value += constant;
        state->locals[index].val.intValue = value;
        state->pc += 2;
        break;
      }
      case I2L: {
        Value val = pop();
        push(Value((long)val.val.intValue));
        break;
      }
      case I2F: {
        Value val = pop();
        push(Value((float)val.val.intValue));
        break;
      }
      case I2D: {
        Value val = pop();
        push(Value((double)val.val.intValue));
        break;
      }
      case L2I: {
        Value val = pop();
        push(Value((int)val.val.longValue));
        break;
      }
      case L2F: {
        Value val = pop();
        push(Value((float)val.val.longValue));
        break;
      }
      case L2D: {
        Value val = pop();
        push(Value((double)val.val.longValue));
        break;
      }
      case F2I: {
        Value val = pop();
        push(Value((int)val.val.floatValue));
        break;
      }
      case F2L: {
        Value val = pop();
        push(Value((long)val.val.floatValue));
        break;
      }
      case F2D: {
        Value val = pop();
        push(Value((double)val.val.floatValue));
        break;
      }
      case D2I: {
        Value val = pop();
        push(Value((int)val.val.doubleValue));
        break;
      }
      case D2L: {
        Value val = pop();
        push(Value((long)val.val.doubleValue));
        break;
      }
      case D2F: {
        Value val = pop();
        push(Value((float)val.val.doubleValue));
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
        cerr << "byte-code " << byteCodeNames.at(mnemonic)
             << " not supported at this moment, please come back later <3"
             << endl;
        break;
      }
    }
    DEBUG_PRINT("{}: {} - Top of stack: {}\n", byteCodeNames.at(mnemonic),
                state->pc,
                !state->stack.empty() ? toString(state->stack.top()) : "-");

    state->pc += 1;
  }
}

void Interpreter::invoke(size_t nameIndex) {
  Method methodToInvoke = program.methods[nameIndex];
  stack<Value> stack;
  map<size_t, Value> locals;
  if (!states.empty()) {
    vector<Type> argTypes = methodToInvoke.argTypes;
    size_t key = sizeOf(argTypes);
    for (int i = argTypes.size() - 1; i >= 0; i--) {
      Type type = argTypes[i];
      key -= sizeOf(type);
      Value toAdd = pop();
      locals[key] = toAdd;
      DEBUG_PRINT("Arg addr: {} = {}\n", key, toString(toAdd));
    }
    assert(key == 0 && "Last (first) argument should end up in the begining");
  }
  DEBUG_PRINT("method {} stack {} locals {} {}\n", nameIndex, stack.size(),
              locals[0].val.intValue, locals[1].val.intValue);
  State *state = new State{0, nameIndex, stack, locals};
  states.push(state);
  eval();
}

size_t Interpreter::findNameIndex(size_t methodRef) {
  ConstantMethodRef *constantMethodRef =
      (ConstantMethodRef *)program.constantPool[methodRef];
  ConstantNameAndType *constantNameAndType =
      (ConstantNameAndType *)
          program.constantPool[constantMethodRef->nameAndTypeIndex];
  return constantNameAndType->nameIndex;
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

void Interpreter::push(Value val) { states.top()->stack.push(val); }

string Interpreter::toString(Value val) {
  switch (val.type.type) {
    case Int:
      return to_string(val.val.intValue);
    case Long:
      return to_string(val.val.longValue);
    case Float:
      return to_string(val.val.floatValue);
    case Double:
      return to_string(val.val.doubleValue);
    default:
      return "VA?!?";
  }
}

int Interpreter::getParametersAsInt() {
  State *state = states.top();
  vector<uint8_t> code = program.methods[state->method].code;
  uint8_t byte1 = code[state->pc + 1];
  uint8_t byte2 = code[state->pc + 2];
  short res = byte1 << 8 | byte2;
  return (int)res;
}

size_t Interpreter::sizeOf(vector<Type> argTypes) {
  size_t size = 0;
  for (Type type : argTypes) {
    size += sizeOf(type);
  }
  return size;
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