#pragma once
#include "connectivity.h"
#include "graph.h"

Forest spanning_forest(
    const Graph &G, double beta,
    function<bool(NodeId, NodeId)> pred = [](NodeId, NodeId) { return true; }) {
  sequence<NodeId> label;
  sequence<pair<NodeId, NodeId>> tree_edges;
  internal::timer t_conn;
  tie(label, tree_edges) = connect(G, beta, pred, true);
  t_conn.next("Connectivity");

  auto label_edge = tabulate(tree_edges.size(), [&](size_t i) {
    return make_tuple(label[tree_edges[i].first], tree_edges[i].first,
                      tree_edges[i].second);
  });
  sort_inplace(make_slice(label_edge), [](tuple<NodeId, NodeId, NodeId> a,
                                          tuple<NodeId, NodeId, NodeId> b) {
    if (get<0>(a) != get<0>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  });
  auto first_vertex = tabulate(label_edge.size(), [&](NodeId i) {
    return static_cast<NodeId>(i == 0 || get<0>(label_edge[i]) !=
                                             get<0>(label_edge[i - 1]));
  });
  // NOTE: trivial nodes are removed
  Forest F;
  F.num_trees = scan_inplace(first_vertex);
  auto first_edge = tabulate(label_edge.size(), [&](NodeId i) {
    return static_cast<NodeId>(i == 0 || get<1>(label_edge[i]) !=
                                             get<1>(label_edge[i - 1]));
  });
  F.G.n = scan_inplace(first_edge);
  F.G.offset = sequence<NodeId>::uninitialized(F.G.n + 1);
  F.offset = sequence<NodeId>::uninitialized(F.num_trees + 1);
  F.vertex = sequence<NodeId>::uninitialized(F.G.n);
  parallel_for(0, label_edge.size(), [&](size_t i) {
    if (i == 0 || get<1>(label_edge[i]) != get<1>(label_edge[i - 1])) {
      F.G.offset[first_edge[i]] = i;
      F.vertex[first_edge[i]] = get<1>(label_edge[i]);
    }
  });
  parallel_for(0, label_edge.size(), [&](size_t i) {
    if (i == 0 || get<0>(label_edge[i]) != get<0>(label_edge[i - 1])) {
      F.offset[first_vertex[i]] = first_edge[i];
    }
  });
  F.offset[F.num_trees] = F.G.n;
  F.G.m = label_edge.size();
  F.G.offset[F.G.n] = F.G.m;
  F.G.E = sequence<NodeId>::uninitialized(label_edge.size());
  parallel_for(0, F.G.m, [&](size_t i) { F.G.E[i] = get<2>(label_edge[i]); });
  return F;
}
