
#include <iostream>

#include <fst/fstapi.h>

using namespace std;

char fstName[] = "../test_data/top.fst";

int main(int argc, char **argv)
{

    void *fstCtx;

    fstCtx = fstReaderOpen((const char *)fstName);
    if (!fstCtx){
        cerr << "Can't open " << fstName << endl;
        exit(-1);
    }

    return 0;
}
