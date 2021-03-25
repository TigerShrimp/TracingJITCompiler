#ifndef LRU_QUEUE_HPP
#define LRU_QUEUE_HPP

#include <list>

#include "x86.hpp"
static const size_t usableRegs = 12;
static const REG allUsableRegs[usableRegs] = {RSI, RDX, RCX, R8,  R9,  R10,
                                              R11, RBX, R12, R13, R14, R15};

class LRUQueue {
 public:
  LRUQueue();
  // Adds REG to the end of list to note that it is now currently the last used.
  void update(REG);
  // Returns the least recently used register
  REG get();

 private:
  std::list<REG> queue;
};
#endif  // LRU_QUEUE_HPP