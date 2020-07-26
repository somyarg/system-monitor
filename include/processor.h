#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  void updatePrevIdle(unsigned long long Idle);
  void updatePrevNonIdle(unsigned long long NonIdle);
  
  unsigned long long PrevNonIdle {0ULL};
  unsigned long long PrevIdle {0ULL};
};

#endif