#ifndef luaSearchingH
#define luaSearchingH

#include <dirent.h>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;

#include "essentials.hpp"
#include "matchWithLua.hpp"

void simpleSearch( string, string, vector< string >&, bool );

#endif
