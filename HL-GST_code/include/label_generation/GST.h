#pragma once
#include <unordered_set>
#include <graph_v_of_v/graph_v_of_v_minimum_spanning_tree.h>
#include <graph_v_of_v/graph_v_of_v_shortest_paths.h>
#include <label_generation/two_hop_labels.h>
#include <label_generation/PLL.h>
#include <label_generation/CT.h>
#include <graph_v_of_v/graph_v_of_v_hop_constrained_shortest_distance.h>
#include <label_generation/hop_constrained_two_hop_labels_generation.h>
#include <text_mining/print_items.h>



vector<pair<int, int>> build_a_tree(graph_v_of_v<int>& instance_graph, unordered_set<int>& vertices_in_tree) {

	/*find an MST*/
	int N = instance_graph.size();
	int min_cost_v = 0;
	graph_v_of_v<int> sub_graph(N);
	for (auto v : vertices_in_tree) {
		min_cost_v = v;
		for (auto adj : instance_graph[v]) {
			if (v < adj.first && vertices_in_tree.count(adj.first) > 0) {
				sub_graph.add_edge(v, adj.first, instance_graph.edge_weight(v, adj.first));
			}
		}
	}
	//sub_graph.print();
	vector<pair<int, int>> edges_in_mst;
	std::vector<int> predecessors = graph_v_of_v_minimum_spanning_tree(sub_graph, min_cost_v);
	for (auto v : vertices_in_tree) {
		if (predecessors[v] != v) {
			edges_in_mst.push_back({ predecessors[v], v });
		}
	}
	//print_a_sequence_of_elements(predecessors);

	return edges_in_mst;
}


template <typename case_info_type>
unordered_set<int> GST_nonHOP(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs) {

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					cost += (long long int)extract_distance(case_info, v, g);
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					vector<pair<int, int>> path;
					extract_shortest_path(case_info, min_cost_v, g, path);
					//cout << "min_cost_v " << min_cost_v << endl;
					//print_vector_pair_int(path);
					for (auto e : path) {
						vertices_in_tree.insert(e.first);
						vertices_in_tree.insert(e.second);
					}
				}
			}
		}
	}

	//auto begin3 = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//double runningtime1 = std::chrono::duration_cast<std::chrono::nanoseconds>(begin3 - begin2).count() / 1e9; // s
	//double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	//cout << runningtime1 / runningtime << endl;
	////getchar();

	return vertices_in_tree;
}

template <typename case_info_type>
unordered_set<int> GST_nonHOP_noMOCK(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs) {

	int max_t = 60; // s
	auto begin0 = std::chrono::high_resolution_clock::now();

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}
					long long int minD = INT_MAX;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)extract_distance(case_info, v, vg.first);
						if (minD > dis) {
							minD = dis;
						}
					}
					cost += minD;
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}
					long long int minD = INT_MAX, minDv = 0;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)extract_distance(case_info, min_cost_v, vg.first);
						if (minD > dis) {
							minD = dis;
							minDv = vg.first;
						}
					}
					vector<pair<int, int>> path;
					extract_shortest_path(case_info, min_cost_v, minDv, path);
					for (auto e : path) {
						vertices_in_tree.insert(e.first);
						vertices_in_tree.insert(e.second);
					}
				}
			}
		}
	}

	return vertices_in_tree;
}

template <typename case_info_type>
pair<double, double> GST_nonHOP_mock_return_Qtime(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs) {

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	double queryD_time = 0, queryP_time = 0;

	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();

		int QD_num = (groups_IDs.size() - 1) * instance_graph[min_groupID].size();
		int QP_num = groups_IDs.size() - 1;

		auto begin = std::chrono::high_resolution_clock::now();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					cost += (long long int)extract_distance(case_info, v, g);
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		queryD_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9 / QD_num;

		if (min_cost < INT_MAX) {
			begin = std::chrono::high_resolution_clock::now();
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					vector<pair<int, int>> path;
					extract_shortest_path(case_info, min_cost_v, g, path);
				}
			}
			queryP_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9 / QP_num;
		}

	}

	return { queryD_time, queryP_time };
}


template <typename case_info_type>
pair<double, double> GST_nonHOP_NOmock_return_Qtime(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs) {

	int max_t = 60; // s
	auto begin0 = std::chrono::high_resolution_clock::now();

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	double queryD_time = 0, queryP_time = 0;

	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();

		double QD_num = (groups_IDs.size() - 1) * instance_graph[min_groupID].size();
		double QP_num = groups_IDs.size() - 1;

		auto begin = std::chrono::high_resolution_clock::now();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {

					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}

					long long int minD = INT_MAX;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)extract_distance(case_info, v, vg.first);
						if (minD > dis) {
							minD = dis;
						}
					}

					cost += minD;
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}

		queryD_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9 / QD_num;

		if (min_cost < INT_MAX) {
			begin = std::chrono::high_resolution_clock::now();
			for (auto g : groups_IDs) {
				if (g != min_groupID) {

					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}

					long long int minD = INT_MAX, minDv = 0;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)extract_distance(case_info, min_cost_v, vg.first);
						if (minD > dis) {
							minD = dis;
							minDv = vg.first;
						}
					}

					vector<pair<int, int>> path;
					extract_shortest_path(case_info, min_cost_v, minDv, path);
				}
			}
			queryP_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9 / QP_num;
		}

	}

	return { queryD_time, queryP_time };
}


unordered_set<int> GST_HOP(hop_constrained_case_info& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs, int diameter_cst) {

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					cost += (long long int)hop_constrained_extract_distance(case_info.L, v, g, diameter_cst / 2);
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					vector<pair<int, int>> path = hop_constrained_extract_shortest_path(case_info.L, min_cost_v, g, diameter_cst / 2);
					for (auto e : path) {
						vertices_in_tree.insert(e.first);
						vertices_in_tree.insert(e.second);
					}
				}
			}
		}
	}

	//auto begin3 = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//double runningtime1 = std::chrono::duration_cast<std::chrono::nanoseconds>(begin3 - begin2).count() / 1e9; // s
	//double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	//cout << runningtime1 / runningtime << endl;
	////getchar();

	return vertices_in_tree;
}


unordered_set<int> GST_HOP_noMOCK(hop_constrained_case_info& case_info, graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs, int diameter_cst) {

	int max_t = 60; // s
	auto begin0 = std::chrono::high_resolution_clock::now();

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}
					long long int minD = INT_MAX;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)hop_constrained_extract_distance(case_info.L, v, vg.first, diameter_cst / 2);
						if (minD > dis) {
							minD = dis;
						}
					}
					cost += minD;
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin0).count() / 1e9 > max_t) {
						throw reach_limit_error_string_time;  // after catching error, must call PLL_clear_global_values(), otherwise PLL cannot be reused
					}
					long long int minD = INT_MAX, minDv = 0;
					for (auto vg : instance_graph[g]) {
						long long int dis = (long long int)hop_constrained_extract_distance(case_info.L, min_cost_v, vg.first, diameter_cst / 2);
						if (minD > dis) {
							minD = dis;
							minDv = vg.first;
						}
					}
					/*query path between min_cost_v and minDv, with diameter_cst / 2*/
					int s = min_cost_v, t = minDv, hop_cst = diameter_cst / 2, path_dis = minD;
					while (s != t && hop_cst) {
						int min_s_adj;
						for (auto s_adj : instance_graph[s]) {
							long long int dis = (long long int)hop_constrained_extract_distance(case_info.L, s_adj.first, t, hop_cst - 1);
							if (dis + s_adj.second == path_dis) {
								s = s_adj.first;								
								path_dis = dis;
								vertices_in_tree.insert(s);
								break;
							}
						}
						hop_cst--;
					}
				}
			}
		}
	}

	//auto begin3 = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//double runningtime1 = std::chrono::duration_cast<std::chrono::nanoseconds>(begin3 - begin2).count() / 1e9; // s
	//double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	//cout << runningtime1 / runningtime << endl;
	////getchar();

	return vertices_in_tree;
}



unordered_set<int> GST_nonHOP_NOINDEX(graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs) {

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	unordered_map<int, std::vector<int>> Distances, Predecessors;
	for (auto g : groups_IDs) {
		if (g != min_groupID) {
			std::vector<int> distances, predecessors;
			graph_v_of_v_shortest_paths<int>(instance_graph, g, distances, predecessors);
			Distances[g] = distances;
			Predecessors[g] = predecessors;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					cost += (long long int)Distances[g][v];
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					int p = min_cost_v;
					while (Predecessors[g][p] != p) {
						vertices_in_tree.insert(p);
						vertices_in_tree.insert(Predecessors[g][p]);
						p = Predecessors[g][p];
					}
				}
			}
		}
	}

	//auto begin3 = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//double runningtime1 = std::chrono::duration_cast<std::chrono::nanoseconds>(begin3 - begin2).count() / 1e9; // s
	//double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	//cout << runningtime1 / runningtime << endl;
	////getchar();

	return vertices_in_tree;
}


unordered_set<int> GST_HOP_NOINDEX(graph_v_of_v<int>& instance_graph, vector<int>& groups_IDs, int diameter_cst) {

	/*find minimum group*/
	int min_groupID, min_groupID_size = INT_MAX;
	for (auto g : groups_IDs) {
		if (instance_graph.degree(g) < min_groupID_size) {
			min_groupID_size = instance_graph.degree(g);
			min_groupID = g;
		}
	}

	unordered_map<int, std::vector<int>> Distances;
	for (auto g : groups_IDs) {
		if (g != min_groupID) {
			std::vector<int> distances;
			graph_v_of_v_hop_constrained_shortest_distance(instance_graph, g, diameter_cst / 2, distances);
			Distances[g] = distances;
		}
	}

	/*find vertices in an approximation solution*/
	unordered_set<int> vertices_in_tree;
	if (instance_graph[min_groupID].size() != 0) {
		int min_cost_v;
		long long int min_cost = std::numeric_limits<long long int>::max();
		for (auto vv : instance_graph[min_groupID]) {
			int v = vv.first;
			long long int cost = 0;
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					cost += (long long int)Distances[g][v];
				}
			}
			if (cost < min_cost) {
				min_cost = cost;
				min_cost_v = v;
			}
		}
		if (min_cost < INT_MAX) {
			vertices_in_tree.insert(min_cost_v);
			for (auto g : groups_IDs) {
				if (g != min_groupID) {
					/*vector<pair<int, int>> path = hop_constrained_extract_shortest_path(case_info.L, min_cost_v, g, diameter_cst / 2);
					for (auto e : path) {
						vertices_in_tree.insert(e.first);
						vertices_in_tree.insert(e.second);
					}*/
				}
			}
		}
	}

	//auto begin3 = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//double runningtime1 = std::chrono::duration_cast<std::chrono::nanoseconds>(begin3 - begin2).count() / 1e9; // s
	//double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	//cout << runningtime1 / runningtime << endl;
	////getchar();

	return vertices_in_tree;
}
