#include <roki_ex/rkex_assembly.h>


void rkAssemblyPartInit(rkAssemblyPart *part)
{
  zNameSetPtr( part, NULL );
  part->chain_file = NULL;
  rkChainInit( &part->chain );
  rkJointInit( &part->joint );
  zFrame3DIdent( &part->frame );

  part->parent = NULL;
  part->parent_link = NULL;
}

void rkAssemblyPartDestroy(rkAssemblyPart *part)
{
  if( !part ) return;
  zNameFree( part );
  zFree( part->chain_file );
  rkChainDestroy( &part->chain );
  rkJointDestroy( &part->joint );
}

void rkAssemblyInit(rkAssembly *assembly)
{
  zNameSetPtr( assembly, NULL );
  zArrayInit( &assembly->parts );
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
  zArrayFree( &assembly->motors );
}

static char *_rkAssemblyPartAddPrefixToName(const char *name, const char *prefix, char *dest, size_t bufsize)
{
  zStrCopy( dest, prefix, bufsize );
  zStrCat( dest, "_", bufsize );
  zStrCat( dest, name, bufsize );
  return dest;
}

void rkAssemblyPartChainAddPrefixToName(rkChain *chain, const char *prefix)
{
  int i;
  char buf[BUFSIZ];
  zMShape3D *ms;
  rkMotorArray *ma;
  rkMotor *m;


  zNameSet( chain, _rkAssemblyPartAddPrefixToName( zName(chain), prefix, buf, BUFSIZ ) );

  for( i=0; i<rkChainLinkNum(chain); i++ ){
    zNameSet( rkChainLink(chain,i), _rkAssemblyPartAddPrefixToName( zName(rkChainLink(chain,i)), prefix, buf, BUFSIZ ) );
    if( (m = rkJointGetMotor(rkChainLinkJoint(chain,i))) )
      zNameSet( m, _rkAssemblyPartAddPrefixToName( zName(m), prefix, buf, BUFSIZ ) );
  }
  
  ms = rkChainShape(chain);
  if( ms ){
    for( i=0; i<zMShape3DShapeNum(ms); i++ )
      zNameSet( zMShape3DShape(ms,i), _rkAssemblyPartAddPrefixToName( zName(zMShape3DShape(ms,i)), prefix, buf, BUFSIZ ) );
    for( i=0; i<zMShape3DOpticNum(ms); i++ )
      zNameSet( zMShape3DOptic(ms,i), _rkAssemblyPartAddPrefixToName( zName(zMShape3DOptic(ms,i)), prefix, buf, BUFSIZ ) );
    for( i=0; i<zMShape3DTextureNum(ms); i++ )
      zNameSet( zMShape3DTexture(ms,i), _rkAssemblyPartAddPrefixToName( zName(zMShape3DTexture(ms,i)), prefix, buf, BUFSIZ ) );
  }

  ma = rkChainMotor(chain);
  if( ma )
    for( i=0; i<zArraySize(ma); i++ )
      zNameSet( zArrayElemNC(ma,i), _rkAssemblyPartAddPrefixToName( zName(zArrayElemNC(ma,i)), prefix, buf, BUFSIZ ) );
}

rkChain *rkAssemblyCreateChain(rkChain *chain)
{
  rkChainInit( chain );
  return chain;
}


/* ZTK */
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
typedef struct{
  const char *dirpath;
  rkAssemblyPartArray *parray;
  rkMotorArray *marray;
} _rkAssemblyPartRefPrp;

static void *_rkAssemblyPartNameFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  rkAssemblyPart *part;
  zArrayFindName( ((_rkAssemblyPartRefPrp*)arg)->parray, ZTKVal(ztk), part );
  if( part ){
    ZRUNWARN( RK_WARN_LINK_DUP, ZTKVal(ztk) );
    return NULL;
  }
  zNameSet( (rkAssemblyPart*)obj, ZTKVal(ztk) );
  return zNamePtr((rkAssemblyPart*)obj) ? obj : NULL;
}

static void _rkAssemblyPartNameFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", zName((rkAssemblyPart*)obj) );
}

static void *_rkAssemblyPartChainFileFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  rkAssemblyPart *part;
  _rkAssemblyPartRefPrp *prp;
  char path[BUFSIZ];

  part = (rkAssemblyPart*)obj;
  prp = (_rkAssemblyPartRefPrp*)arg;

  part->chain_file = zStrClone( ZTKVal(ztk) );
  zStrCopy( path, prp->dirpath, BUFSIZ );
  zStrCat( path, part->chain_file, BUFSIZ );
  if( !rkChainReadZTK( rkAssemblyPartChain(part), path ) ){
    ZRUNWARN( "cannot open chain file" );
    return NULL;
  }
  rkAssemblyPartChainAddPrefixToName( rkAssemblyPartChain(part), zName(part) );
  return part;
}

static void _rkAssemblyPartChainFileFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", rkAssemblyPartChainFile((rkAssemblyPart*)obj) );
}

static void *_rkAssemblyPartJointTypeFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return rkJointQueryAssign( rkAssemblyPartJoint((rkAssemblyPart*)obj), ZTKVal(ztk) );
}

static void _rkAssemblyPartJointTypeFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", rkJointTypeStr( rkAssemblyPartJoint((rkAssemblyPart*)obj)) );
}

static void *_rkAssemblyPartPosFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zVec3DFromZTK( rkAssemblyPartPos((rkAssemblyPart*)obj), ztk );
  return obj;
}

static void _rkAssemblyPartPosFPrintZTK(FILE *fp, int i, void *obj){
  zVec3DFPrint( fp, rkAssemblyPartPos((rkAssemblyPart*)obj) );
}

static void *_rkAssemblyPartAttFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zMat3DFromZTK( rkAssemblyPartAtt((rkAssemblyPart*)obj), ztk );
  return obj;
}

static void _rkAssemblyPartAttFPrintZTK(FILE *fp, int i, void *obj){
  zMat3DFPrint( fp, rkAssemblyPartAtt((rkAssemblyPart*)obj) );
}

static void *_rkAssemblyPartParentFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyPart*)obj)->parent = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyPartParentFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyPartParent((rkAssemblyPart *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static void *_rkAssemblyPartParentLinkFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkAssemblyPart*)obj)->parent_link = zStrClone( ZTKVal(ztk) );
  return obj;
}

static void _rkAssemblyPartParentLinkFPrintZTK(FILE *fp, int i, void *obj){
  char *name = rkAssemblyPartParentLink((rkAssemblyPart *)obj);
  if( name )
    fprintf( fp, "%s\n",  name );
}

static ZTKPrp __ztk_prp_rkassemblypart[] = {
  { "name", 1, _rkAssemblyPartNameFromZTK, _rkAssemblyPartNameFPrintZTK },
  { "chainfile", 1, _rkAssemblyPartChainFileFromZTK, _rkAssemblyPartChainFileFPrintZTK },
  { "parent", 1, _rkAssemblyPartParentFromZTK, _rkAssemblyPartParentFPrintZTK },
  { "parentlink", 1, _rkAssemblyPartParentLinkFromZTK, _rkAssemblyPartParentLinkFPrintZTK },
  { "jointtype", 1, _rkAssemblyPartJointTypeFromZTK, _rkAssemblyPartJointTypeFPrintZTK },
  { "pos", 1, _rkAssemblyPartPosFromZTK, _rkAssemblyPartPosFPrintZTK },
  { "att", 1, _rkAssemblyPartAttFromZTK, _rkAssemblyPartAttFPrintZTK },
};

static void *_rkAssemblyPartFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  _rkAssemblyPartRefPrp *prp;
  rkAssembly *assembly;
  rkAssemblyPart *part;

  assembly = (rkAssembly*)obj;
  part = rkAssemblyGetPart(assembly,i);
  prp = (_rkAssemblyPartRefPrp*)arg;

  rkAssemblyPartInit( part );
  if( !ZTKEvalKey( part, prp, ztk, __ztk_prp_rkassemblypart ) ) return NULL;
  if( !rkAssemblyPartJoint(part)->com ) rkJointAssign( rkAssemblyPartJoint(part), &rk_joint_fixed );
  rkJointFromZTK( rkAssemblyPartJoint(part), prp->marray, ztk );

  if( !part->parent || !part->parent_link ){
    if( assembly->root ) return NULL;
    assembly->root = part;
  }
  
  return part;
}

void _rkAssemblyPartFPrintZTK(FILE *fp, int i, void *obj)
{
  rkAssemblyPart *part;

  part = rkAssemblyGetPart((rkAssembly *)obj,i);
  ZTKPrpKeyFPrint( fp, part, __ztk_prp_rkassemblypart );
  rkJointFPrintZTK( fp, rkAssemblyPartJoint(part), zName(part) );
  fprintf( fp, "\n" );
}

static ZTKPrp __ztk_prp_tag_rkassembly_part[] = {
  { ZTK_TAG_RKEX_ASSEMBLY_PART, -1, _rkAssemblyPartFromZTK, _rkAssemblyPartFPrintZTK },
};

/* public functions */
rkAssembly *rkAssemblyFromZTK(rkAssembly *assembly, ZTK *ztk, const char *dirpath)
{
  _rkAssemblyPartRefPrp prp;
  int num_motor, num_part;

  if( ( num_motor = ZTKCountTag( ztk, ZTK_TAG_RKMOTOR ) ) > 0 ){
    zArrayAlloc( &assembly->motors, rkMotor, num_motor );
    if( rkAssemblyMotorNum(assembly) != num_motor ) return NULL;
  }
  num_part = ZTKCountTag( ztk, ZTK_TAG_RKEX_ASSEMBLY_PART );
  zArrayAlloc( &assembly->parts, rkAssemblyPart, num_part );
  if( rkAssemblyPartNum(assembly) != num_part ) return NULL;
  if( rkAssemblyPartNum(assembly) == 0 ) return NULL;

  prp.dirpath = dirpath;
  prp.marray = &assembly->motors;
  prp.parray = &assembly->parts;

  ZTKEvalTag( assembly, NULL, ztk, __ztk_prp_tag_rkassembly_motor );
  ZTKEvalTag( assembly, &prp, ztk, __ztk_prp_tag_rkassembly_part);
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
