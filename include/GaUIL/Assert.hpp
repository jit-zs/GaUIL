#ifndef GAUIL_ASSERT_HPP
#define GAUIL_ASSERT_HPP


#include <exception>
#include <cstdio>
#define GAUIL_ASSERT(Cond, Msg)\
    if (!(Cond)){\
        printf("Assertion failed[%s][%s](%d): %s\n", __FILE__, __FUNCTION__, __LINE__, Msg);\
        std::terminate();\
    }

#endif