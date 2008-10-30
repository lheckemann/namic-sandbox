#ifndef ARTLINUXCLOCK__H
#define ARTLINUXCLOCK__H

#ifdef __linux__ //if __linux__
// timing checks; NOTE: not reliable for SMP kernels
// the following values are specific for the test machine
#define CPU_CLOCK_PER_USEC 1.794573e+03 //[MHz]=[clock/usec]                    
#define CPU_USEC_PER_CLOCK 5.572356e-04 //[usec/clock]                          
#include <stdint.h>
// NOTE: the following code is from http://en.wikipedia.org/wiki/RDTSC
extern "C" {
  __inline__ uint64_t rdtsc() {
    uint32_t lo, hi;
    /* We cannot use "=A", since this would use %rax on x86_64 */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
  }
}
#endif //if __linux__

#endif //ARTLINUXCLOCK
