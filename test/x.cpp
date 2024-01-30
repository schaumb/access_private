#include "a.hpp"
#include "../include/access_private.hpp"

namespace access_private {
  template struct access<&A::m_i>;
}
