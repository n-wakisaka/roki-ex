#include <roki_ex/rkex_assembly.h>


void rkAssemblyPartInit(rkAssemblyPart *part)
{
  zNameSetPtr( part, NULL );
  part->chain_file = NULL;
  rkChainInit( &part->chain );
}

void rkAssemblyPartDestroy(rkAssemblyPart *part)
{
  if( !part ) return;
  zNameFree( part );
  zFree( part->chain_file );
}

char *rkAssemblyPartAddPrefixToName(const char *name, const char *prefix, char *dest, size_t bufsize)
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


  zNameSet( chain, rkAssemblyPartAddPrefixToName( zName(chain), prefix, buf, BUFSIZ ) );

  for( i=0; i<rkChainLinkNum(chain); i++ ){
    zNameSet( rkChainLink(chain,i), rkAssemblyPartAddPrefixToName( zName(rkChainLink(chain,i)), prefix, buf, BUFSIZ ) );
    if( (m = rkJointGetMotor(rkChainLinkJoint(chain,i))) )
      zNameSet( m, rkAssemblyPartAddPrefixToName( zName(m), prefix, buf, BUFSIZ ) );
  }
  
  ms = rkChainShape(chain);
  if( ms ){
    for( i=0; i<zMShape3DShapeNum(ms); i++ )
      zNameSet( zMShape3DShape(ms,i), rkAssemblyPartAddPrefixToName( zName(zMShape3DShape(ms,i)), prefix, buf, BUFSIZ ) );
    for( i=0; i<zMShape3DOpticNum(ms); i++ )
      zNameSet( zMShape3DOptic(ms,i), rkAssemblyPartAddPrefixToName( zName(zMShape3DOptic(ms,i)), prefix, buf, BUFSIZ ) );
    for( i=0; i<zMShape3DTextureNum(ms); i++ )
      zNameSet( zMShape3DTexture(ms,i), rkAssemblyPartAddPrefixToName( zName(zMShape3DTexture(ms,i)), prefix, buf, BUFSIZ ) );
  }

  ma = rkChainMotor(chain);
  if( ma )
    for( i=0; i<zArraySize(ma); i++ )
      zNameSet( zArrayElemNC(ma,i), rkAssemblyPartAddPrefixToName( zName(zArrayElemNC(ma,i)), prefix, buf, BUFSIZ ) );
}

/* ZTK */
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
    ZRUNWARN( RKEX_WARN_PART_DUP, ZTKVal(ztk) );
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
    ZRUNWARN( RKEX_WARN_INVALID_CHAIN );
    return NULL;
  }
  rkAssemblyPartChainAddPrefixToName( rkAssemblyPartChain(part), zName(part) );
  return part;
}

static void _rkAssemblyPartChainFileFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", rkAssemblyPartChainFile((rkAssemblyPart*)obj) );
}

static ZTKPrp __ztk_prp_rkassemblypart[] = {
  { "name", 1, _rkAssemblyPartNameFromZTK, _rkAssemblyPartNameFPrintZTK },
  { "chainfile", 1, _rkAssemblyPartChainFileFromZTK, _rkAssemblyPartChainFileFPrintZTK },
};

rkAssemblyPart *rkAssemblyPartFromZTK(rkAssemblyPart *part, rkAssemblyPartArray *parray, const char *dirpath, ZTK *ztk)
{
  _rkAssemblyPartRefPrp prp;

  prp.dirpath = dirpath;
  prp.parray = parray;

  rkAssemblyPartInit( part );
  if( !ZTKEvalKey( part, &prp, ztk, __ztk_prp_rkassemblypart ) ) return NULL;

  return part;
}

void rkAssemblyPartFPrintZTK(FILE *fp, rkAssemblyPart *part)
{
  ZTKPrpKeyFPrint( fp, part, __ztk_prp_rkassemblypart );
  fprintf( fp, "\n" );
}
