#include <roki_ex/roki_ex.h>

int main(int argc, char *argv[])
{
  rkAssembly assembly;
  rkChain chain;

  eprintf("read\n");
  rkAssemblyReadZTK( &assembly, "../model/arm_box.ztk" );
  rkAssemblyFPrintZTK( stdout, &assembly );

  eprintf("chain\n");
  rkAssemblyCreateChain( &assembly, &chain );
  rkChainFPrintZTK( stdout, &chain );

  eprintf("destroy\n");
  rkChainDestroy( &chain );
  rkAssemblyDestroy( &assembly );
  
  return 0;
}