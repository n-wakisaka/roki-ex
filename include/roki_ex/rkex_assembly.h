#ifndef __RKEX_ASSEMBLY_H__
#define __RKEX_ASSEMBLY_H__

#include <roki_ex/rkex_defs.h>
#include <roki_ex/rkex_assembly_part.h>
#include <roki_ex/rkex_assembly_joint.h>

__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_EX_CLASS_EXPORT, rkAssembly) {
  Z_NAMED_CLASS;
  rkAssemblyPart *root;
  rkAssemblyPartArray parts;
  rkAssemblyJointArray joints;
  rkMotorArray motors;
};

#define rkAssemblyParts(a) ( &(a)->parts )
#define rkAssemblyPartNum(a) zArraySize( &(a)->parts )
#define rkAssemblyGetPart(a,i) zArrayElemNC( &(a)->parts, i )
#define rkAssemblyJoints(a) ( &(a)->joints )
#define rkAssemblyJointNum(a) zArraySize( &(a)->joints )
#define rkAssemblyGetJoint(a,i) zArrayElemNC( &(a)->joints, i )
#define rkAssemblyMotors(a) ( &(a)->motors )
#define rkAssemblyMotorNum(a) zArraySize( &(a)->motors )
#define rkAssemblyGetMotor(a,i) zArrayElemNC( &(a)->motors, i )

__ROKI_EX_EXPORT void rkAssemblyInit(rkAssembly *assembly);
__ROKI_EX_EXPORT void rkAssemblyDestroy(rkAssembly *assembly);

__ROKI_EX_EXPORT rkChain *rkAssemblyCreateChain(rkAssembly *assembly, rkChain *chain);


#define ZTK_TAG_RKEX_ASSEMBLY "assembly"

__ROKI_EX_EXPORT rkAssembly *rkAssemblyFromZTK(rkAssembly *assembly, ZTK *ztk, const char *dirpath);
__ROKI_EX_EXPORT void rkAssemblyFPrintZTK(FILE *fp, rkAssembly *assembly);

__ROKI_EX_EXPORT rkAssembly *rkAssemblyReadZTK(rkAssembly *assembly, const char *filename);
__ROKI_EX_EXPORT bool rkAssemblyWriteZTK(rkAssembly *assembly, const char *filename);

__END_DECLS

#endif /* __RKEX_CHAIN_BUILDER_H__ */
