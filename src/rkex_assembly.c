#include <roki_ex/rkex_assembly.h>


void rkAssemblyInit(rkAssembly *assembly)
{
  zNameSetPtr( assembly, NULL );
  zArrayInit( &assembly->parts );
  zArrayInit( &assembly->joints );
  zArrayInit( &assembly->motors );
  assembly->root = NULL;
}

void rkAssemblyDestroy(rkAssembly *assembly)
{
  int i;

  if( !assembly ) return;
  zNameFree( assembly );
  for( i=0; i<rkAssemblyPartNum(assembly); i++ )
    rkAssemblyPartDestroy( rkAssemblyGetPart(assembly,i) );
  zArrayFree( &assembly->parts );
  for( i=0; i<rkAssemblyJointNum(assembly); i++ )
    rkAssemblyJointDestroy( rkAssemblyGetJoint(assembly,i) );
  zArrayFree( &assembly->joints );
  zArrayFree( &assembly->motors );
}

static rkChain *_rkAssemblyChainAlloc(rkAssembly *assembly, rkChain *chain)
{
  int i, num_link, num_shape, num_optic, num_texture, num_motor;
  rkChain *c;
  zMShape3D *ms;
  rkMotorArray *marray;

  /* count */
  num_link = 0;
  num_shape = 0;
  num_optic = 0;
  num_texture = 0;
  num_motor = rkAssemblyMotorNum(assembly);
  for( i=0; i<rkAssemblyPartNum(assembly); i++ ){
    c = rkAssemblyPartChain(rkAssemblyGetPart(assembly,i));
    num_link += rkChainLinkNum(c);
    if( (ms = rkChainShape(c)) ){
      num_shape += zMShape3DShapeNum(ms);
      num_optic += zMShape3DOpticNum(ms);
      num_texture += zMShape3DTextureNum(ms);
    }
    if( (marray = rkChainMotor(c)) )
      num_motor += zArraySize(marray);
  }

  /* alloc */
  zArrayAlloc( rkChainLinkArray(chain), rkLink, num_link );
  if( rkChainLinkNum(chain) != num_link ) return NULL;
  if( rkChainLinkNum(chain) == 0 ){
    ZRUNWARN( RK_WARN_CHAIN_EMPTY );
    return NULL;
  }
  if( num_shape > 0){
    if( !( rkChainShape(chain) = zAlloc( zMShape3D, 1 ) ) ){
      ZALLOCERROR();
      return NULL;
    }
    ms = rkChainShape(chain);
    zMShape3DInit( ms );
    zArrayAlloc( &ms->shape, zShape3D, num_shape );
    zArrayAlloc( &ms->optic, zOpticalInfo, num_optic );
    zArrayAlloc( &ms->texture, zTexture, num_texture );
  }
  if( num_motor > 0 ){
    if( !( rkChainMotor(chain) = zAlloc( rkMotorArray, 1 ) ) ){
      ZALLOCERROR();
      return NULL;
    }
    marray = rkChainMotor(chain);
    zArrayInit( marray );
    zArrayAlloc( marray, rkMotor, num_motor );
    if( zArraySize(marray) != num_motor ) return NULL;
  }

  return chain;
}

static rkBody *_rkAssemblyChainCloneBody(rkBody *org, rkBody *cln, zShape3D *so, zShape3D *sc)
{
  zShapeListCell *sp;

  rkMPCopy( &org->mp, &cln->mp );
  zListInit( rkBodyExtWrench(cln) );
  zListInit( rkBodyShapeList(cln) );
  zListForEach( rkBodyShapeList(org), sp )
    if( !rkBodyShapePush( cln, sp->data - so + sc ) ) return NULL;
  if( rkBodyStuff(org) && !rkBodySetStuff( cln, rkBodyStuff(org) ) ){
    ZALLOCERROR();
    return NULL;
  }
  return cln;
}

static rkLink *_rkAssemblyChainCloneLink(rkLink *org, rkLink *cln, zShape3D *so, zShape3D *sc)
{
  if( !zNameSet( cln, zName(org) ) ||
      !rkJointClone( rkLinkJoint(org), rkLinkJoint(cln) ) ||
      !_rkAssemblyChainCloneBody( rkLinkBody(org), rkLinkBody(cln), so, sc ) ){
    ZALLOCERROR();
    return NULL;
  }
  rkMPCopy( rkLinkCRB(org), rkLinkCRB(cln) );
  rkLinkSetOrgFrame( cln, rkLinkOrgFrame(org) );

  rkLinkSetParent( cln,
    rkLinkParent(org) ? rkLinkParent(org) - org + cln : NULL );
  rkLinkSetChild( cln,
    rkLinkChild(org) ? rkLinkChild(org) - org + cln : NULL );
  rkLinkSetSibl( cln,
    rkLinkSibl(org) ? rkLinkSibl(org) - org + cln : NULL );
  return cln;
}

static zShape3D *_rkAssemblyChainCloneShape3D(zShape3D *org, zShape3D *cln, zOpticalInfo *oi, zTexture *ot)
{
  if( !zNameSet( cln, zName(org) ) ){
    ZALLOCERROR();
    return NULL;
  }
  if( !( cln->body = ( cln->com = org->com )->_clone( org->body ) ) )
    return NULL;
  zShape3DSetOptic( cln, oi );
  zShape3DSetTexture( cln, ot );
  return cln;
}

static rkChain *_rkAssemblyChainClone(rkAssembly *assembly, rkChain *chain)
{
  int i, j, num_link, num_shape, num_optic, num_texture, num_motor;
  rkChain *c;
  zMShape3D *mshape, *ms;
  rkMotorArray *marray;

  /* assembly motor */
  for( i=0; i<rkAssemblyMotorNum(assembly); i++ )
    if( !rkMotorClone( rkAssemblyGetMotor(assembly,i), zArrayElemNC(rkChainMotor(chain),i) ) )
      return NULL;

  /* assembly parts */
  num_link = 0;
  num_shape = 0;
  num_optic = 0;
  num_texture = 0;
  num_motor = rkAssemblyMotorNum(assembly);
  mshape = rkChainShape(chain);
  for( i=0; i<rkAssemblyPartNum(assembly); i++ ){
    c = rkAssemblyPartChain(rkAssemblyGetPart(assembly,i));
    ms = rkChainShape(c);
    marray = rkChainMotor(c);

    /* link */
    for( j=0; j<rkChainLinkNum(c); j++ ){
      if( !_rkAssemblyChainCloneLink( rkChainLink(c,j), rkChainLink(chain,num_link+j),
          zMShape3DShapeBuf(ms), zMShape3DShapeBuf(mshape)+num_shape ) )
        return NULL;
    }
    num_link += rkChainLinkNum(c);

    /* mshape */
    if( ms ){
      for( j=0; j<zMShape3DOpticNum(ms); j++ )
        if( !zOpticalInfoClone( zMShape3DOptic(ms,j), zMShape3DOptic(mshape,num_optic+j) ) )
          return NULL;
      for( j=0; j<zMShape3DTextureNum(ms); j++ )
        if( !zTextureClone( zMShape3DTexture(ms,j), zMShape3DTexture(mshape,num_texture+j) ) )
          return NULL;
      for( j=0; j<zMShape3DShapeNum(ms); j++ )
        if( !_rkAssemblyChainCloneShape3D( zMShape3DShape(ms,j), zMShape3DShape(mshape,num_shape+j),
              zMShape3DOptic( mshape, num_optic+zShape3DOptic(zMShape3DShape(ms,j)) - zMShape3DOpticBuf(ms) ),
              zMShape3DTexture( mshape, num_texture+zShape3DTexture(zMShape3DShape(ms,j)) - zMShape3DTextureBuf(ms) ) ) )
          return NULL;
      num_optic += zMShape3DOpticNum(ms);
      num_texture += zMShape3DTextureNum(ms);
      num_shape += zMShape3DShapeNum(ms);
    }

    /* motor */
    if( marray ){
      for( j=0; j<zArraySize(marray); j++ )
        if( !rkMotorClone( zArrayElemNC(marray,j), zArrayElemNC(rkChainMotor(chain),num_motor+j) ) )
          return NULL;
      num_motor += zArraySize(marray);
    }
  }

  return chain;
}

rkLink *_rkAssemblyLinkPurgeFromParentChildren(rkLink *link)
{
  rkLink *parent, *child;

  if( !link || !( parent = rkLinkParent(link) ) ) return link;  
  if( ( child = rkLinkChild(parent) ) == link ){
    rkLinkChild(parent) = rkLinkSibl(link);
    return link;
  }
  if( !child || !rkLinkSibl(child) ) return NULL; /* link does not exist in parent children */
  for( ; rkLinkSibl(child); child=rkLinkSibl(child) )
    if( rkLinkSibl(child) == link ){
      rkLinkSibl(child) = rkLinkSibl(link);
      return link;
    }
  
  return NULL;
}

rkLink *_rkAssemblyLinkChangeRoot(rkLink *link, rkLink *new_parent, rkJoint *new_joint, zFrame3D* orgframe)
{
  rkLink *l, *parent;

  if( ( parent = rkLinkParent(link) ) ){
    _rkAssemblyLinkChangeRoot( parent, link, rkLinkJoint(link), rkLinkOrgFrame(link) );
    _rkAssemblyLinkPurgeFromParentChildren( link );
    rkLinkAddChild( link, parent );
  }
  for( l=rkLinkChild(link); l; l=rkLinkSibl(l) )
    zFrame3DXform( orgframe, rkLinkOrgFrame(l), rkLinkOrgFrame(l) );
  
  rkLinkSetParent( link, new_parent );
  rkJointDestroy( rkLinkJoint(link) );
  rkJointClone( new_joint, rkLinkJoint(link) );

  return link;
}

rkChain *_rkAssemblyChainConnection(rkAssembly *assembly, rkChain *chain)
{
  int i;
  rkAssemblyJoint *aj;
  rkLink *parent, *child;
  char name[BUFSIZ];

  for( i=0; i<rkAssemblyJointNum(assembly); i++ ){
    aj = rkAssemblyGetJoint(assembly,i);
    rkAssemblyPartAddPrefixToName( rkAssemblyJointParentLink(aj), rkAssemblyJointParent(aj), name, BUFSIZ );
    if( !( parent = rkChainFindLink( chain, name ) ) ) return NULL;
    rkAssemblyPartAddPrefixToName( rkAssemblyJointChildLink(aj), rkAssemblyJointChild(aj), name, BUFSIZ );
    if( !( child = rkChainFindLink( chain, name ) ) ) return NULL;

    _rkAssemblyLinkChangeRoot( child, parent, rkAssemblyJointJoint(aj), rkAssemblyJointChildFrame(aj) );
    rkLinkSetOrgFrame( child, rkAssemblyJointParentFrame(aj) );
  }

  return chain;
}

rkChain *rkAssemblyCreateChain(rkAssembly *assembly, rkChain *chain)
{
  rkChainInit( chain );
  if( !zNameSet( chain, zName(assembly) ) ) return NULL;
  if( !_rkAssemblyChainAlloc( assembly, chain ) ||
      !_rkAssemblyChainClone( assembly, chain ) ||
      !_rkAssemblyChainConnection( assembly, chain ))
    return NULL;
  rkChainSetJointIDOffset( chain );

  return chain;
}

/* ZTK */

typedef struct{
  const char *dirpath;
} _rkAssemblyRefPrp;

/* [assembly] */
static void *_rkAssemblyNameFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return zNameSet( (rkAssembly*)obj, ZTKVal(ztk) ) ? obj : NULL;
}

static void _rkAssemblyNameFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", zName((rkAssembly*)obj) );
}

static ZTKPrp __ztk_prp_rkassembly_assembly[] = {
  { "name", 1, _rkAssemblyNameFromZTK, _rkAssemblyNameFPrintZTK },
};

static void *_rkAssemblyAssemblyFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ZTKEvalKey( obj, arg, ztk, __ztk_prp_rkassembly_assembly );
}

static void _rkAssemblyAssemblyFPrintZTK(FILE *fp, int i, void *obj){
  ZTKPrpKeyFPrint( fp, obj, __ztk_prp_rkassembly_assembly );
  fprintf( fp, "\n" );
}

static ZTKPrp __ztk_prp_tag_rkassembly[] = {
  { ZTK_TAG_RKEX_ASSEMBLY, 1, _rkAssemblyAssemblyFromZTK, _rkAssemblyAssemblyFPrintZTK },
};

/* [motor] */
static void *_rkAssemblyMotorFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return rkMotorFromZTK( rkAssemblyGetMotor((rkAssembly*)obj,i), ztk );
}

static ZTKPrp __ztk_prp_tag_rkassembly_motor[] = {
  { ZTK_TAG_RKMOTOR, -1, _rkAssemblyMotorFromZTK, NULL },
};

/* [part] */
static void *_rkAssemblyPartFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return rkAssemblyPartFromZTK( rkAssemblyGetPart((rkAssembly*)obj,i), 
    rkAssemblyParts((rkAssembly*)obj), ((_rkAssemblyRefPrp*)arg)->dirpath, ztk )
    ? obj : NULL;
}

void _rkAssemblyPartFPrintZTK(FILE *fp, int i, void *obj)
{
  rkAssemblyPartFPrintZTK( fp, rkAssemblyGetPart((rkAssembly*)obj,i) );
}

static ZTKPrp __ztk_prp_tag_rkassembly_part[] = {
  { ZTK_TAG_RKEX_ASSEMBLY_PART, -1, _rkAssemblyPartFromZTK, _rkAssemblyPartFPrintZTK },
};

/* [joint] */
static void *_rkAssemblyJointFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return rkAssemblyJointFromZTK( rkAssemblyGetJoint((rkAssembly*)obj,i),
    rkAssemblyParts((rkAssembly*)obj), rkAssemblyMotors((rkAssembly*)obj),
    ((_rkAssemblyRefPrp*)arg)->dirpath, ztk )
    ? obj : NULL;
}

void _rkAssemblyJointFPrintZTK(FILE *fp, int i, void *obj)
{
  rkAssemblyJointFPrintZTK( fp, rkAssemblyGetJoint((rkAssembly*)obj,i) );
}

static ZTKPrp __ztk_prp_tag_rkassembly_joint[] = {
  { ZTK_TAG_RKEX_ASSEMBLY_JOINT, -1, _rkAssemblyJointFromZTK, _rkAssemblyJointFPrintZTK },
};

/* public functions */
rkAssembly *rkAssemblyFromZTK(rkAssembly *assembly, ZTK *ztk, const char *dirpath)
{
  _rkAssemblyRefPrp prp;
  int num_motor, num_part, num_joint;

  if( ( num_motor = ZTKCountTag( ztk, ZTK_TAG_RKMOTOR ) ) > 0 ){
    zArrayAlloc( &assembly->motors, rkMotor, num_motor );
    if( rkAssemblyMotorNum(assembly) != num_motor ) return NULL;
  }

  num_part = ZTKCountTag( ztk, ZTK_TAG_RKEX_ASSEMBLY_PART );
  zArrayAlloc( &assembly->parts, rkAssemblyPart, num_part );
  if( rkAssemblyPartNum(assembly) != num_part ) return NULL;
  if( rkAssemblyPartNum(assembly) == 0 ) return NULL;

  num_joint = ZTKCountTag( ztk, ZTK_TAG_RKEX_ASSEMBLY_JOINT );
  zArrayAlloc( &assembly->joints, rkAssemblyJoint, num_joint );
  if( rkAssemblyJointNum(assembly) != num_joint ) return NULL;
  if( rkAssemblyJointNum(assembly) == 0 ) return NULL;

  prp.dirpath = dirpath;
  ZTKEvalTag( assembly, NULL, ztk, __ztk_prp_tag_rkassembly_motor );
  ZTKEvalTag( assembly, &prp, ztk, __ztk_prp_tag_rkassembly_part);
  ZTKEvalTag( assembly, &prp, ztk, __ztk_prp_tag_rkassembly_joint);
  ZTKEvalTag( assembly, NULL, ztk, __ztk_prp_tag_rkassembly );

  return assembly;
}

void rkAssemblyFPrintZTK(FILE *fp, rkAssembly *assembly)
{
  ZTKPrp prp[1];

  ZTKPrpTagFPrint( fp, assembly, __ztk_prp_tag_rkassembly );
  rkMotorArrayFPrintZTK( fp, &assembly->motors );

  prp[0] = __ztk_prp_tag_rkassembly_part[0];
  prp[0].num = rkAssemblyPartNum(assembly);
  ZTKPrpTagFPrint( fp, assembly, prp );

  prp[0] = __ztk_prp_tag_rkassembly_joint[0];
  prp[0].num = rkAssemblyJointNum(assembly);
  ZTKPrpTagFPrint( fp, assembly, prp );
}

static char *_rkAssemblyGetDirname(const char *filename, const char *def, char *dirname, size_t buffsize){
  char buf[BUFSIZ], _filename[BUFSIZ];
  int retval;

  zStrCopy( buf, filename, BUFSIZ );
  retval = zGetDirFilename( buf, dirname, _filename, buffsize );
  if (!(retval & 0b10)) zStrCopy( dirname, def, buffsize );
  return dirname;
}

rkAssembly *rkAssemblyReadZTK(rkAssembly *assembly, const char *filename)
{
  ZTK ztk;
  char dirname[BUFSIZ];

  ZTKInit( &ztk );
  if( ZTKParse( &ztk, (char *)filename ) ){
    rkAssemblyInit( assembly );
    assembly = rkAssemblyFromZTK( assembly, &ztk, _rkAssemblyGetDirname( filename, "", dirname, BUFSIZ ) );
  } else
    assembly = NULL;
  ZTKDestroy( &ztk );
  return assembly;
}

bool rkAssemblyWriteZTK(rkAssembly *assembly, const char *filename)
{
  FILE *fp;

  if( !( fp = zOpenZTKFile( (char *)filename, "w" ) ) ) return false;
  rkAssemblyFPrintZTK( fp, assembly );
  fclose(fp);
  return true;
}
