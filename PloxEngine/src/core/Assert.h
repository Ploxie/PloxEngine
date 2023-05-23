//
// Created by Ploxie on 2023-05-10.
//

#define ASSERT_ENABLED

#ifdef ASSERT_ENABLED
#if _MSC_VER

#include <intrin.h>

#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

void ReportAssertionFailure(const char* expression, const char* message, const char* file, unsigned int line);

#define ASSERT(expr)                                                                                             \
        {                                                                                                              \
            if (expr) { }                                                                                              \
            else                                                                                                       \
            {                                                                                                          \
                ReportAssertionFailure(#expr, "", __FILE__, __LINE__);                                               \
                debugBreak();                                                                                          \
            }                                                                                                          \
        }

#define ASSERT_MSG(expr, message)                                                                                \
        {                                                                                                              \
            if (expr) { }                                                                                              \
            else                                                                                                       \
            {                                                                                                          \
                ReportAssertionFailure(#expr, message, __FILE__, __LINE__);                                          \
                debugBreak();                                                                                          \
            }                                                                                                          \
        }

#ifdef _DEBUG
#define ASSERT_DEBUG(expr)                                                                                   \
            {                                                                                                          \
                if (expr) { }                                                                                          \
                else                                                                                                   \
                {                                                                                                      \
                    ReportAssertionFailure(#expr, "", __FILE__, __LINE__);                                           \
                    debugBreak();                                                                                      \
                }                                                                                                      \
            }
#else
#define P_ASSERT_DEBUG(expr)
#endif

#else
#define P_ASSERT(expr)
#define P_ASSERT_MSG(expr, message)
#define P_ASSERT_DEBUG(expr)
#endif

