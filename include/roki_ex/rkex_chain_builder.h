#ifndef __RKEX_CHAIN_BUILDER_H__
#define __RKEX_CHAIN_BUILDER_H__

#include <roki_ex/rkex_defs.h>

#include <roki/rk_chain.h>


__BEGIN_DECLS

typedef struct _rkChainBuilder {
} rkChainBuilder;

#define ZTK_TAG_RKCHAIN "chain"

__ROKI_EX_EXPORT rkChainBuilder *rkChainBuilderFromZTK(rkChainBuilder *builder, ZTK *ztk);
__ROKI_EX_EXPORT void rkChainBuilderFPrintZTK(FILE *fp, rkChainBuilder *builder);

__ROKI_EX_EXPORT rkChainBuilder *rkChainBuilderReadZTK(rkChainBuilder *builder, const char *filename);
__ROKI_EX_EXPORT bool rkChainBuilderWriteZTK(rkChainBuilder *builder, const char *filename);

__END_DECLS

#endif /* __RKEX_CHAIN_BUILDER_H__ */