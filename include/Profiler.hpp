#ifndef PROFILER_HPP
#define PROFILER_HPP
#include <map>

class Profiler {
 public:
  bool isHot(size_t, size_t);
  void note(size_t, size_t);

 private:
  static const size_t hotThreshold = 2;
  size_t lastMethod;
  size_t lastPc;
  std::map<size_t, std::map<size_t, size_t> > loopRecord;
};

#endif  // PROFILER_HPP