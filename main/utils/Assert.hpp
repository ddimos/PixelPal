#pragma once

#if CONFIG_PP_DISABLE_ASSERT
#    undef PP_ASSERT
#    define PP_ASSERT(condition, message) (void(0))
#elif !defined PP_ASSERT
#    include <cassert>
#    define PP_ASSERT(condition, message) assert(condition && message)
#endif
