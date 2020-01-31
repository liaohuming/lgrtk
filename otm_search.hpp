#pragma once

namespace lgr {
namespace search {

void initialize_otm_search();

void finalize_otm_search();

void do_otm_point_node_search(lgr::state &s, int max_support_nodes_per_point);

void invert_otm_point_node_relations(lgr::state & s);

}
}