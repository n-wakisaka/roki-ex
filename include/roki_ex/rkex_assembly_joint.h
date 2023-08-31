#ifndef __RKEX_ASSEMBLY_JOINT_H__
#define __RKEX_ASSEMBLY_JOINT_H__

#include <roki_ex/rkex_assembly_part.h>

#include <roki/rk_chain.h>

__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_EX_CLASS_EXPORT, rkAssemblyJoint ) {
  rkJoint joint;

  char *parent;
  char *parent_link;
  zFrame3D parent_frame;

  char *child;
  char *child_link;
  zFrame3D child_frame;
};

zArrayClass( rkAssemblyJointArray, rkAssemblyJoint );

#define rkAssemblyJointJoint(j) ( &(j)->joint )
#define rkAssemblyJointFrame(j) ( &(j)->frame )
#define rkAssemblyJointParent(j) ( (j)->parent )
#define rkAssemblyJointParentLink(j) ( (j)->parent_link )
#define rkAssemblyJointParentPos(j) ( zFrame3DPos(&(j)->parent_frame) )
#define rkAssemblyJointParentAtt(j) ( zFrame3DAtt(&(j)->parent_frame) )
#define rkAssemblyJointChild(j) ( (j)->child )
#define rkAssemblyJointChildLink(j) ( (j)->child_link )
#define rkAssemblyJointChildPos(j) ( zFrame3DPos(&(j)->child_frame) )
#define rkAssemblyJointChildAtt(j) ( zFrame3DAtt(&(j)->child_frame) )

__ROKI_EX_EXPORT void rkAssemblyJointInit(rkAssemblyJoint *joint);
__ROKI_EX_EXPORT void rkAssemblyJointDestroy(rkAssemblyJoint *joint);

__ROKI_EX_EXPORT bool rkAssemblyJointValidateConnection(rkAssemblyJoint *joint, rkAssemblyPartArray *parray);

/* ZTK */

#define ZTK_TAG_RKEX_ASSEMBLY_JOINT "joint"

__ROKI_EX_EXPORT rkAssemblyJoint *rkAssemblyJointFromZTK(rkAssemblyJoint *joint, rkAssemblyPartArray *parray, rkMotorArray *marray, const char *dirpath, ZTK *ztk);
__ROKI_EX_EXPORT void rkAssemblyJointFPrintZTK(FILE *fp, rkAssemblyJoint *joint);

__END_DECLS

#endif /* __RKEX_ASSEMBLY_JOINT_H__ */
