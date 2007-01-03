#ifndef RUNNABLE_TEST_H_
#define RUNNABLE_TEST_H_

class RunnableTest
{
protected:
  virtual void Register() = 0;

public:
  int RunTest();
};


#endif /*RUNNABLE_TEST_H_*/
