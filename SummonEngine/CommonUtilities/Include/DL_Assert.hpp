#ifndef DL_ASSERT_HEADER
#define DL_ASSERT_HEADER

#include "DL_Debug.hpp"
#include <crtdefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);

#ifdef  __cplusplus
}
#endif

#ifdef assert
#undef assert
#endif


#define assert(_Expression) \
  if (! (_Expression)) \
  { \
	DL_ASSERT(#_Expression);\
	_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__); \
  }

/*

#undef  assert

#ifdef  NDEBUG

#define assert(_Expression)     ((void)0)

#else

#ifdef  __cplusplus
extern "C" {
#endif

_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);

#ifdef  __cplusplus
}
#endif

#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )

#endif  /* NDEBUG */


#endif
