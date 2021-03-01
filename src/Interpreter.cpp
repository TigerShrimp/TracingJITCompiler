#include "Interpreter.hpp"

using namespace std;

void Interpreter::eval(Program *program) {
  State *state = program->states.top();
  Mnemonic mnemonic = static_cast<Mnemonic>(program->readNext());
  switch (mnemonic) {
    // Constants loading
    case ICONST_M1: {
      program->push(Value(-1));
      break;
    }
    case ICONST_0: {
      program->push(Value(0));
      break;
    }
    case ICONST_1: {
      program->push(Value(1));
      break;
    }
    case ICONST_2: {
      program->push(Value(2));
      break;
    }
    case ICONST_3: {
      program->push(Value(3));
      break;
    }
    case ICONST_4: {
      program->push(Value(4));
      break;
    }
    case ICONST_5: {
      program->push(Value(5));
      break;
    }
    case LCONST_0: {
      program->push(Value(0L));
      break;
    }
    case LCONST_1: {
      program->push(Value(1L));
      break;
    }
    case FCONST_0: {
      program->push(Value(0.0f));
      break;
    }
    case FCONST_1: {
      program->push(Value(1.0f));
      break;
    }
    case FCONST_2: {
      program->push(Value(2.0f));
      break;
    }
    case DCONST_0: {
      program->push(Value(0.0));
      break;
    }
    case DCONST_1: {
      program->push(Value(1.0));
      break;
    }
    case BIPUSH: {
      uint8_t byte = program->readNext();
      int8_t sByte = (int8_t)byte;
      program->push(Value(sByte));
      break;
    }
    case SIPUSH: {
      int param = readParametersAsInt(program);
      program->push(Value(param));
      break;
    }
    // Loading
    case LDC: {
      int index = program->readNext();
      CPInfo *cpInfoEntry = program->constantPool[index];
      if (cpInfoEntry->info() == "ConstantFloat: ") {
        ConstantFloat *constantFloat = (ConstantFloat *)cpInfoEntry;
        float floatRes;
        memcpy(&floatRes, &constantFloat->bytes, sizeof(float));
        program->push(Value(floatRes));
      } else {  // ConstantInt
        ConstantInteger *constantInteger = (ConstantInteger *)cpInfoEntry;
        int intRes = (int)constantInteger->bytes;
        program->push(Value(intRes));
      }
      break;
    }
    case LDC2_W: {
      // TODO: Make Long/Double constants more general
      int index = readParametersAsInt(program);
      CPInfo *cpInfoEntry = program->constantPool[index];
      if (cpInfoEntry->info() == "ConstantDouble: ") {
        ConstantDouble *constantDouble = (ConstantDouble *)cpInfoEntry;
        // The high and low bytes are stored in different positions of the
        // constant pool so they have to be combined into one 64-bit number
        long longRes =
            ((long)constantDouble->highBytes << 32) + constantDouble->lowBytes;
        double doubleRes;
        memcpy(&doubleRes, &longRes, sizeof(double));
        program->push(Value(doubleRes));
      } else {  // ConstantLong
        ConstantLong *constantLong = (ConstantLong *)cpInfoEntry;
        long longRes =
            ((long)constantLong->highBytes << 32) + constantLong->lowBytes;
        program->push(Value(longRes));
      }
      break;
    }
    case ILOAD:
    case LLOAD:
    case FLOAD:
    case DLOAD: {
      int index = program->readNext();
      program->load(index);
      break;
    }
    case ILOAD_0:
    case LLOAD_0:
    case FLOAD_0:
    case DLOAD_0: {
      program->load(0);
      break;
    }
    case ILOAD_1:
    case LLOAD_1:
    case FLOAD_1:
    case DLOAD_1: {
      program->load(1);
      break;
    }
    case ILOAD_2:
    case LLOAD_2:
    case FLOAD_2:
    case DLOAD_2: {
      program->load(2);
      break;
    }
    case ILOAD_3:
    case LLOAD_3:
    case FLOAD_3:
    case DLOAD_3: {
      program->load(3);
      break;
    }
    // Storing
    case ISTORE:
    case LSTORE:
    case FSTORE:
    case DSTORE: {
      int index = program->readNext();
      program->store(index);
      break;
    }
    case ISTORE_0:
    case LSTORE_0:
    case FSTORE_0:
    case DSTORE_0: {
      program->store(0);
      break;
    }
    case ISTORE_1:
    case LSTORE_1:
    case FSTORE_1:
    case DSTORE_1: {
      program->store(1);
      break;
    }
    case ISTORE_2:
    case LSTORE_2:
    case FSTORE_2:
    case DSTORE_2: {
      program->store(2);
      break;
    }
    case ISTORE_3:
    case LSTORE_3:
    case FSTORE_3:
    case DSTORE_3: {
      program->store(3);
      break;
    }
    // Comparisons
    case LCMP:
    case FCMPL:
    case FCMPG:
    case DCMPL:
    case DCMPG: {
      Value rhs = program->pop();
      Value lhs = program->pop();
      if (lhs < rhs)
        program->push(Value(-1));
      else if (rhs < lhs)
        program->push(Value(1));
      else
        program->push(Value(0));
      break;
    }
    // Control flow
    case IFGT: {
      int value = program->pop().val.intValue;
      int offset = readParametersAsInt(program);
      if (value > 0) program->jump(offset, 3);
      break;
    }
    case IF_ICMPGE: {
      int rhs = program->pop().val.intValue;
      int lhs = program->pop().val.intValue;
      int offset = readParametersAsInt(program);
      if (lhs >= rhs) program->jump(offset, 3);
      break;
    }
    case IF_ICMPGT: {
      int rhs = program->pop().val.intValue;
      int lhs = program->pop().val.intValue;
      int offset = readParametersAsInt(program);
      if (lhs > rhs) program->jump(offset, 3);
      break;
    }
    case GOTO: {
      int offset = readParametersAsInt(program);
      program->jump(offset, 3);
      break;
    }
    case RETURN: {
      State *state = program->states.top();
      program->states.pop();
      delete state;
      return;
    }
    case IRETURN:
    case LRETURN:
    case FRETURN:
    case DRETURN: {
      State *state = program->states.top();
      program->states.pop();
      Value returnValue = state->stack.top();
      delete state;
      program->states.top()->stack.push(returnValue);
      return;
    }
    case INVOKEVIRTUAL: {
      // Is currently only used when System.out.println is called
      Value val = program->pop();
      cout << toString(val) << endl;
      program->jump(2);
      break;
    }
    case INVOKESTATIC: {
      int methodRefIndex = readParametersAsInt(program);
      size_t nameIndex = findNameIndex(program, methodRefIndex);
      invoke(program, nameIndex);
      break;
    }
    // Arithmetic
    case IADD:
    case LADD:
    case FADD:
    case DADD: {
      Value rhs = program->pop();
      Value lhs = program->pop();
      program->push(lhs + rhs);
      break;
    }
    case ISUB:
    case LSUB:
    case FSUB:
    case DSUB: {
      Value rhs = program->pop();
      Value lhs = program->pop();
      program->push(lhs - rhs);
      break;
    }
    case IMUL:
    case LMUL:
    case FMUL:
    case DMUL: {
      Value rhs = program->pop();
      Value lhs = program->pop();
      program->push(lhs * rhs);
      break;
    }
    case IDIV:
    case LDIV:
    case FDIV:
    case DDIV: {
      Value rhs = program->pop();
      Value lhs = program->pop();
      program->push(lhs / rhs);
      break;
    }
    case IINC: {
      uint8_t index = program->readNext();
      int8_t constant = program->readNext();
      int value = state->locals[index].val.intValue;
      value += constant;
      state->locals[index].val.intValue = value;
      break;
    }
    case I2L: {
      Value val = program->pop();
      program->push(Value((long)val.val.intValue));
      break;
    }
    case I2F: {
      Value val = program->pop();
      program->push(Value((float)val.val.intValue));
      break;
    }
    case I2D: {
      Value val = program->pop();
      program->push(Value((double)val.val.intValue));
      break;
    }
    case L2I: {
      Value val = program->pop();
      program->push(Value((int)val.val.longValue));
      break;
    }
    case L2F: {
      Value val = program->pop();
      program->push(Value((float)val.val.longValue));
      break;
    }
    case L2D: {
      Value val = program->pop();
      program->push(Value((double)val.val.longValue));
      break;
    }
    case F2I: {
      Value val = program->pop();
      program->push(Value((int)val.val.floatValue));
      break;
    }
    case F2L: {
      Value val = program->pop();
      program->push(Value((long)val.val.floatValue));
      break;
    }
    case F2D: {
      Value val = program->pop();
      program->push(Value((double)val.val.floatValue));
      break;
    }
    case D2I: {
      Value val = program->pop();
      program->push(Value((int)val.val.doubleValue));
      break;
    }
    case D2L: {
      Value val = program->pop();
      program->push(Value((long)val.val.doubleValue));
      break;
    }
    case D2F: {
      Value val = program->pop();
      program->push(Value((float)val.val.doubleValue));
      break;
    }
    // Misc
    case GETSTATIC: {
      // int index = readParametersAsInt();
      // ConstantFieldRef* value =
      // (ConstantFieldRef*)program->constantPool[index]; value->classIndex;
      program->jump(2);
      break;
    }
    case NOP:
      break;
    default: {
      cerr << "byte-code " << mnemonic << byteCodeNames.at(mnemonic)
           << " not supported at this moment, please come back later <3"
           << endl;
      break;
    }
  }
  DEBUG_PRINT("{}: {} - Top of stack: {}\n", byteCodeNames.at(mnemonic),
              state->pc,
              !state->stack.empty() ? toString(state->stack.top()) : "-");
  // TODO: change how we handle the pc to instead move it forward in different
  // read functions similar to "readU1" in the parser
}

void Interpreter::invoke(Program *program, size_t nameIndex) {
  Method methodToInvoke = program->methods[nameIndex];
  stack<Value> stack;
  map<size_t, Value> locals;
  vector<Type> argTypes = methodToInvoke.argTypes;
  size_t key = sizeOf(argTypes);
  for (int i = argTypes.size() - 1; i >= 0; i--) {
    Type type = argTypes[i];
    key -= sizeOf(type);
    Value toAdd = program->pop();
    locals[key] = toAdd;
    DEBUG_PRINT("Arg addr: {} = {}\n", key, toString(toAdd));
  }
  assert(key == 0 && "Last (first) argument should end up in the begining");
  DEBUG_PRINT("method {} stack {} locals {} {}\n", nameIndex, stack.size(),
              locals[0].val.intValue, locals[1].val.intValue);
  State *state = new State{0, nameIndex, stack, locals};
  program->states.push(state);
}

size_t Interpreter::findNameIndex(Program *program, size_t methodRef) {
  ConstantMethodRef *constantMethodRef =
      (ConstantMethodRef *)program->constantPool[methodRef];
  ConstantNameAndType *constantNameAndType =
      (ConstantNameAndType *)
          program->constantPool[constantMethodRef->nameAndTypeIndex];
  return constantNameAndType->nameIndex;
}

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

int Interpreter::readParametersAsInt(Program *program) {
  uint8_t byte1 = program->readNext();
  uint8_t byte2 = program->readNext();
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