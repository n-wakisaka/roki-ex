/* ../include/roki_ex/roki_ex_export.h */
/* This file was automatically generated. */
/* 2023年  8月 30日 水曜日 03:08:58 JST by natsuru */
#ifndef __ROKI_EX_EXPORT_H__
#define __ROKI_EX_EXPORT_H__
#include <zeda/zeda_compat.h>
#if defined(__WINDOWS__) && !defined(__CYGWIN__)
# if defined(__ROKI_EX_BUILD_DLL__)
#  define __ROKI_EX_EXPORT extern __declspec(dllexport)
#  define __ROKI_EX_CLASS_EXPORT  __declspec(dllexport)
# else
#  define __ROKI_EX_EXPORT extern __declspec(dllimport)
#  define __ROKI_EX_CLASS_EXPORT  __declspec(dllimport)
# endif
#else
# define __ROKI_EX_EXPORT __EXPORT
# define __ROKI_EX_CLASS_EXPORT
#endif
#endif /* __ROKI_EX_EXPORT_H__ */
