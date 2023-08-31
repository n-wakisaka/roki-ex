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

rkChain *rkAssemblyCreateChain(rkChain *chain)
{
  rkChainInit( chain );
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
