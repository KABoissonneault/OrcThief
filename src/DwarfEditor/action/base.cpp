#include "base.h"

#include "core/fwd_delete.h"

namespace ot::dedit::action
{
	base::~base() = default;
}

template struct ot::fwd_delete<ot::dedit::action::base>;
