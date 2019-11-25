#include "sd.h"

void SDThread::main() {
    setName("sd");
    if (m_fs.start()) {
        return;
    }
}