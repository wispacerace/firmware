#pragma once
#include <ch.hpp>

#include "filesystem.h"

using namespace chibios_rt;

class SDThread : public BaseStaticThread<4096> {
  public:
    SDThread() {}

  protected:
    void main() override;

  private:
    FilesystemComponent m_fs;
};
