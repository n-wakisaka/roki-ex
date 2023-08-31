#include <roki_ex/roki_ex.h>

int main(int argc, char *argv[])
{
  rkAssembly assembly;

  eprintf("read\n");
  rkAssemblyReadZTK( &assembly, "../model/arm_box.ztk" );
  rkAssemblyFPrintZTK( stdout, &assembly );

  eprintf("destroy\n");
  rkAssemblyDestroy( &assembly );
  
  return 0;
}