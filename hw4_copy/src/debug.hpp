#ifndef DEBUG_HPP
#define DEBUG_HPP

// ONLY DEFINE DEBUG IN CPP FILES!!!
// ALSO MAKE SURE DEBUG IS DEFINED AT THE VERY TOP!!!! (otherwise it won't work)

#ifdef DEBUG_PRINT
#define DEBUG_printf(...)  printf(__VA_ARGS__);
#else
#define DEBUG_printf(...)
#endif

#ifdef DEBUG_ASSERT
#define DEBUG_assert(args)  assert(args);
#else
#define DEBUG_assert(args)
#endif

#endif