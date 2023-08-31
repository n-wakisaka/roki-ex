#ifndef __RKEX_ASSEMBLY_PART_H__
#define __RKEX_ASSEMBLY_PART_H__

#include <roki_ex/rkex_misc.h>

#include <roki/rk_chain.h>

__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_EX_CLASS_EXPORT, rkAssemblyPart ) {
  Z_NAMED_CLASS;
  char *chain_file;
  rkChain chain;
};

zArrayClass( rkAssemblyPartArray, rkAssemblyPart );

#define rkAssemblyPartChainFile(p) ( (p)->chain_file ? (p)->chain_file : (char *)"nofile" )
#define rkAssemblyPartChain(p) ( &(p)->chain )

__ROKI_EX_EXPORT void rkAssemblyPartInit(rkAssemblyPart *part);
__ROKI_EX_EXPORT void rkAssemblyPartDestroy(rkAssemblyPart *part);

__ROKI_EX_EXPORT char *rkAssemblyPartAddPrefixToName(const char *name, const char *prefix, char *dest, size_t bufsize);
__ROKI_EX_EXPORT void rkAssemblyPartChainAddPrefixToName(rkChain *chain, const char* prefix);

/* ZTK */

#define ZTK_TAG_RKEX_ASSEMBLY_PART "part"

__ROKI_EX_EXPORT rkAssemblyPart *rkAssemblyPartFromZTK(rkAssemblyPart *part, rkAssemblyPartArray *parray, const char *dirpath, ZTK *ztk);
__ROKI_EX_EXPORT void rkAssemblyPartFPrintZTK(FILE *fp, rkAssemblyPart *part);

__END_DECLS

#endif /* __RKEX_ASSEMBLY_PART_H__ */
