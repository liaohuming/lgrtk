#pragma once

namespace lgr {

class input;
class state;

void update_quality(input const& in, state& s);
bool adapt(input const& in, state& s);

}
