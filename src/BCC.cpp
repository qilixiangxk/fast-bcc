#include "BCC.h"

#include <iostream>

#include "graph.h"
#include "hopcroft_tarjan.h"
int NUM_ROUNDS = 10;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " filename\n";
    abort();
  }
  char* filename = argv[1];
  if (argc == 3) {
    NUM_ROUNDS = atoi(argv[2]);
  }
  ofstream ofs("fast-bcc.csv", ios_base::app);
  ofs << filename << ',';
  ofs.close();
  internal::timer t_g;
  Graph g = read_graph(filename);
  printf("Graph read: %f\n", t_g.total_time());
  double total_time = 0;
  for (int i = 0; i <= NUM_ROUNDS; i++) {
    if (i == 0) {
      BCC solver(g);
      internal::timer t_critical;
      solver.biconnectivity();
      t_critical.stop();
      printf("Warmup round: %f\n", t_critical.total_time());
    } else {
      BCC solver(g);
      internal::timer t_critical;
      solver.biconnectivity();
      t_critical.stop();
      printf("Round %d: %f\n", i, t_critical.total_time());
      total_time += t_critical.total_time();
    }
  }
  printf("Average time: %f\n", total_time / NUM_ROUNDS);
  BCC solver(g);
  auto label = solver.biconnectivity();
  solver.get_num_bcc(label);
  ofs.open("fast-bcc.csv", ios_base::app);
  ofs << total_time / NUM_ROUNDS << '\n';
  ofs.close();
  // if (true) {
  // BCC solver(g);
  // auto label = solver.biconnectivity();
  // solver.get_num_bcc(label);

  // auto v1 = solver.get_articulation_point(label);
  // sort_inplace(make_slice(v1));
  // internal::timer t_tarjan;
  // Tarjan solver2(g);
  // solver2.bcc();
  // auto v2 = vector<NodeId>(solver2.articulation_point.begin(),
  // solver2.articulation_point.end());
  // sort(begin(v2), end(v2));
  // printf("Tarjan running time: %f\n", t_tarjan.total_time());
  // printf("v1.size()=%zu, v2.size()=%zu\n", v1.size(), v2.size());
  // assert(v1.size() == v2.size());
  // for (size_t i = 0; i < v1.size(); i++) {
  // if (v1[i] != v2[i]) {
  // printf("my point: %u, tarjan's point: %u\n", v1[i], v2[i]);
  // puts("Wrong Answer");
  // return 0;
  //}
  //}
  //}
  return 0;
}
