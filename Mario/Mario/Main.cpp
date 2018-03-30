#include <tchar.h>
#include "SuperMarioGame.h"

int _tmain(int argc, _TCHAR* argv[])
{
   CMarioGame::instance()->run();
   return 0;
}


