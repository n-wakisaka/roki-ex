#ifndef __RKEX_ASSEMBLY_H__
#define __RKEX_ASSEMBLY_H__

#include <roki_ex/rkex_defs.h>

#include <roki/rk_chain.h>


__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_EX_CLASS_EXPORT, rkAssemblyPart ) {
  Z_NAMED_CLASS;
  char *chain_file;
  rkChain chain;
  rkJoint joint;
  zFrame3D frame;

  rkAssemblyPart *parent;
  rkLink *parent_link;
};

#define rkAssemblyPartChainFile(p) ( (p)->chain_file ? (p)->chain_file : (char *)"nofile" )
#define rkAssemblyPartChain(p) ( &(p)->chain )
#define rkAssemblyPartJoint(p) ( &(p)->joint )
#define rkAssemblyPartFrame(p) ( &(p)->frame )
#define rkAssemblyPartPos(p) ( zFrame3DPos(&(p)->frame) )
#define rkAssemblyPartAtt(p) ( zFrame3DAtt(&(p)->frame) )
#define rkAssemblyPartParent(p) ( (p)->parent )
#define rkAssemblyPartParentLink(p) ( (p)->parent_link )

__ROKI_EX_EXPORT void rkAssemblyPartInit(rkAssemblyPart *part);
__ROKI_EX_EXPORT void rkAssemblyPartDestroy(rkAssemblyPart *part);


zArrayClass( rkAssemblyPartArray, rkAssemblyPart );

ZDEF_STRUCT( __ROKI_EX_CLASS_EXPORT, rkAssembly) {
  Z_NAMED_CLASS;
  rkAssemblyPartArray parts;
  rkMotorArray motors;
};

#define rkAssemblyParts(a) ( &(a)->parts )
#define rkAssemblyPartNum(a) zArraySize( &(a)->parts )
#define rkAssemblyGetPart(a,i) zArrayElemNC( &(a)->parts, i )
#define rkAssemblyMotors(a) ( &(a)->motors )
#define rkAssemblyMotorNum(a) zArraySize( &(a)->motors )
#define rkAssemblyGetMotor(a,i) zArrayElemNC( &(a)->motors, i )

__ROKI_EX_EXPORT void rkAssemblyInit(rkAssembly *assembly);
__ROKI_EX_EXPORT void rkAssemblyDestroy(rkAssembly *assembly);

__ROKI_EX_EXPORT rkChain *rkAssemblyCreateChain(rkChain *chain);


#define ZTK_TAG_RKEX_ASSEMBLY "assembly"
#define ZTK_TAG_RKEX_ASSEMBLY_PART "part"

__ROKI_EX_EXPORT rkAssembly *rkAssemblyFromZTK(rkAssembly *assembly, ZTK *ztk, const char *dirpath);
__ROKI_EX_EXPORT void rkAssemblyFPrintZTK(FILE *fp, rkAssembly *assembly);

__ROKI_EX_EXPORT rkAssembly *rkAssemblyReadZTK(rkAssembly *assembly, const char *filename);
__ROKI_EX_EXPORT bool rkAssemblyWriteZTK(rkAssembly *assembly, const char *filename);

__END_DECLS

#endif /* __RKEX_CHAIN_BUILDER_H__ */