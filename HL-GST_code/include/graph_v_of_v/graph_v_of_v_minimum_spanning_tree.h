
#pragma once
#include <vector>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <boost/heap/fibonacci_heap.hpp> 
#include <graph_v_of_v/graph_v_of_v.h>

using namespace std;


struct graph_v_of_v_node_for_mst {
	int index;
	double priority_value;
}; // define the node in the queue
bool operator<(graph_v_of_v_node_for_mst const& x, graph_v_of_v_node_for_mst const& y) {
	return x.priority_value > y.priority_value; // < is the max-heap; > is the min heap
}
typedef typename boost::heap::fibonacci_heap<graph_v_of_v_node_for_mst>::handle_type handle_t_for_graph_v_of_v_mst;


template<typename T> // T is float or double
std::vector<int> graph_v_of_v_minimum_spanning_tree(graph_v_of_v<T>& input_graph, int root = 0) {

	T inf = std::numeric_limits<T>::max();

	int N = input_graph.size();
	std::vector<int> predecessors_in_mst(N);
	std::iota(std::begin(predecessors_in_mst), std::end(predecessors_in_mst), 0); // initial predecessor of each vertex is itself

	graph_v_of_v_node_for_mst node;
	boost::heap::fibonacci_heap<graph_v_of_v_node_for_mst> Q;
	std::vector<T> Q_keys(N, inf); // if the key of a vertex is inf, then it is not in Q yet
	std::vector<handle_t_for_graph_v_of_v_mst> Q_handles(N);

	/*initialize the source*/
	Q_keys[root] = 0;
	node.index = root;
	node.priority_value = 0;
	Q_handles[root] = Q.push(node);

	while (Q.size() > 0) {

		int top_v = Q.top().index;
		T top_key = Q.top().priority_value;
		Q_keys[top_v] = -1;

		Q.pop();

		for (auto it = input_graph.ADJs[top_v].begin(); it != input_graph.ADJs[top_v].end(); it++) {
			int adj_v = it->first;
			T ec = it->second;
			if (Q_keys[adj_v] == inf) { // adj_v is not in Q yet
				Q_keys[adj_v] = ec;
				node.index = adj_v;
				node.priority_value = Q_keys[adj_v];
				Q_handles[adj_v] = Q.push(node);
				predecessors_in_mst[adj_v] = top_v;
			}
			else if (Q_keys[adj_v] > ec) { // needs to update key
				Q_keys[adj_v] = ec;
				node.index = adj_v;
				node.priority_value = Q_keys[adj_v];
				Q.update(Q_handles[adj_v], node);
				predecessors_in_mst[adj_v] = top_v;
			}
		}
	}

	return predecessors_in_mst;
}