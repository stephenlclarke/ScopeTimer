#include "ScopeTimer.hpp"
#include <unistd.h>

int main() {
    SCOPE_TIMER("Steve's Test");

    usleep(2500);

    return 0;
}
