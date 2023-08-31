#include <roki_ex/rkex_assembly.h>


void rkAssemblyJointInit(rkAssemblyJoint *joint)
{
  rkJointInit( &joint->joint );

  joint->parent = NULL;
  joint->parent_link = NULL;
  zFrame3DIdent( &joint->parent_frame );

  joint->child = NULL;
  joint->child_link = NULL;
  zFrame3DIdent( &joint->child_frame );
}

void rkAssemblyJointDestroy(rkAssemblyJoint *joint)
{
  if( !joint ) return;
  rkJointDestroy( &joint->joint );
  zFree( joint->parent );
  zFree( joint->parent_link );
  zFree( joint->child );
  zFree( joint->child_link );
}

static bool _rkAssemblyJointValidateConnection(const char *part_name, const char *link_name, rkAssemblyPartArray *parray)
{
  rkAssemblyPart *part;
  char buf[BUFSIZ];

  if( !part_name || !link_name ) return false;
  zArrayFindName( parray, part_name, part );
  if( !part ) return false;
  rkAssemblyPartAddPrefixToName( link_name, part_name, buf, BUFSIZ );
  return rkChainFindLink( rkAssemblyPartChain(part), buf ) ? true : false;
}

bool rkAssemblyJointValidateConnection(rkAssemblyJoint *joint, rkAssemblyPartArray *parray)
{
  if( !joint || !parray ) return false;
  return 
    _rkAssemblyJointValidateConnection( rkAssemblyJointParent(joint), rkAssemblyJointParentLink(joint), parray) &&
    _rkAssemblyJointValidateConnection( rkAssemblyJointChild(joint), rkAssemblyJointChildLink(joint), parray);
}

/* ZTK */

typedef struct{
  const char *dirpath;
  rkAssemblyPartArray *parray;
} _rkAssemblyJointRefPrp;

static void *_rkAssemblyJointParentFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyJoint*)obj)->parent = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyJointParentFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyJointParent((rkAssemblyJoint *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static void *_rkAssemblyJointParentLinkFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyJoint*)obj)->parent_link = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyJointParentLinkFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyJointParentLink((rkAssemblyJoint *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static void *_rkAssemblyJointParentPosFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zVec3DFromZTK( rkAssemblyJointParentPos((rkAssemblyJoint*)obj), ztk );
  return obj;
}

static void _rkAssemblyJointParentPosFPrintZTK(FILE *fp, int i, void *obj){
  zVec3DFPrint( fp, rkAssemblyJointParentPos((rkAssemblyJoint*)obj) );
}

static void *_rkAssemblyJointParentAttFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zMat3DFromZTK( rkAssemblyJointParentAtt((rkAssemblyJoint*)obj), ztk );
  return obj;
}

static void _rkAssemblyJointParentAttFPrintZTK(FILE *fp, int i, void *obj){
  zMat3DFPrint( fp, rkAssemblyJointParentAtt((rkAssemblyJoint*)obj) );
}

static void *_rkAssemblyJointChildFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyJoint*)obj)->child = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyJointChildFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyJointChild((rkAssemblyJoint *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static void *_rkAssemblyJointChildLinkFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyJoint*)obj)->child_link = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyJointChildLinkFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyJointChildLink((rkAssemblyJoint *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static void *_rkAssemblyJointChildPosFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zVec3DFromZTK( rkAssemblyJointChildPos((rkAssemblyJoint*)obj), ztk );
  return obj;
}

static void _rkAssemblyJointChildPosFPrintZTK(FILE *fp, int i, void *obj){
  zVec3DFPrint( fp, rkAssemblyJointChildPos((rkAssemblyJoint*)obj) );
}

static void *_rkAssemblyJointChildAttFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zMat3DFromZTK( rkAssemblyJointChildAtt((rkAssemblyJoint*)obj), ztk );
  return obj;
}

static void _rkAssemblyJointChildAttFPrintZTK(FILE *fp, int i, void *obj){
  zMat3DFPrint( fp, rkAssemblyJointChildAtt((rkAssemblyJoint*)obj) );
}

static void *_rkAssemblyJointJointTypeFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return rkJointQueryAssign( rkAssemblyJointJoint((rkAssemblyJoint*)obj), ZTKVal(ztk) );
}

static void _rkAssemblyJointJointTypeFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", rkJointTypeStr( rkAssemblyJointJoint((rkAssemblyJoint*)obj)) );
}

static ZTKPrp __ztk_prp_rkassemblyjoint[] = {
  { "parent", 1, _rkAssemblyJointParentFromZTK, _rkAssemblyJointParentFPrintZTK },
  { "parentlink", 1, _rkAssemblyJointParentLinkFromZTK, _rkAssemblyJointParentLinkFPrintZTK },
  { "parentpos", 1, _rkAssemblyJointParentPosFromZTK, _rkAssemblyJointParentPosFPrintZTK },
  { "parentatt", 1, _rkAssemblyJointParentAttFromZTK, _rkAssemblyJointParentAttFPrintZTK },
  { "child", 1, _rkAssemblyJointChildFromZTK, _rkAssemblyJointChildFPrintZTK },
  { "childlink", 1, _rkAssemblyJointChildLinkFromZTK, _rkAssemblyJointChildLinkFPrintZTK },
  { "childpos", 1, _rkAssemblyJointChildPosFromZTK, _rkAssemblyJointChildPosFPrintZTK },
  { "childatt", 1, _rkAssemblyJointChildAttFromZTK, _rkAssemblyJointChildAttFPrintZTK },
  { "jointtype", 1, _rkAssemblyJointJointTypeFromZTK, _rkAssemblyJointJointTypeFPrintZTK },
};

rkAssemblyJoint *rkAssemblyJointFromZTK(rkAssemblyJoint *joint, rkAssemblyPartArray *parray, rkMotorArray *marray, const char *dirpath, ZTK *ztk)
{
  _rkAssemblyJointRefPrp prp;

  prp.dirpath = dirpath;
  prp.parray = parray;

  rkAssemblyJointInit( joint );
  if( !ZTKEvalKey( joint, &prp, ztk, __ztk_prp_rkassemblyjoint ) ) return NULL;
  if( !rkAssemblyJointValidateConnection( joint, parray ) ){
    ZRUNWARN( RKEX_WARN_INVALID_LINK_CONNECTION );
    return NULL;
  }
  if( !rkAssemblyJointJoint(joint)->com ) rkJointAssign( rkAssemblyJointJoint(joint), &rk_joint_fixed );
  rkJointFromZTK( rkAssemblyJointJoint(joint), marray, ztk );

  return joint;
}

void rkAssemblyJointFPrintZTK(FILE *fp, rkAssemblyJoint *joint)
{
  ZTKPrpKeyFPrint( fp, joint, __ztk_prp_rkassemblyjoint );
  rkJointFPrintZTK( fp, rkAssemblyJointJoint(joint), NULL );
  fprintf( fp, "\n" );  
}
