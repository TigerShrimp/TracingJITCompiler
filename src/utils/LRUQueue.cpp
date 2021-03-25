#include "utils/LRUQueue.hpp"

LRUQueue::LRUQueue() : queue(allUsableRegs, allUsableRegs + usableRegs) {}

void LRUQueue::update(REG registerToAdd) {
  queue.remove_if([=](auto reg) { return reg == registerToAdd; });
  queue.push_back(registerToAdd);
}

REG LRUQueue::get() {
  REG reg = queue.front();
  queue.pop_front();
  return reg;
}