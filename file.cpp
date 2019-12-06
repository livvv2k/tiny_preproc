// test
#include "file.h"

#define TTTT 1

#ifdef TTTT
int zz = 1000;
#endif

#ifdef YYYY
int yy = 999;
#elif TTTT
int xx = 888;
#endif

int main()
{
    if (TEST_MACRO || TEST_MACRO_2 || TTTT)
        return 1;
    return 0;
}
