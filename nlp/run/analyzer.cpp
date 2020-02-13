// Automatically generated: Tue Nov 07 15:42:21 2006
#include "analyzer.h"
#include "ehead.h"
#include "rhead.h"
#include "data.h"
#include "../user/user.h"
#include "../user/Ucode.h"

bool run_analyzer(Parse * parse) {
  if (!Arun::init_analyzer(parse)) return false;
  tokenize1(parse);
  return true;
}
bool tokenize1(Parse * parse) {
  if (!Arun::tokenize(parse, 1)) return false;
  return true;
}
