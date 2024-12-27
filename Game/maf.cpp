#include "maf.h"

maf::ivec2::operator maf::fvec2()
{
	return { (float)this->x, (float)this->y };
}
