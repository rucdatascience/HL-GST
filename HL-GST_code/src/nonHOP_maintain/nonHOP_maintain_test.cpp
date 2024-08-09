#include <iostream>
#include <fstream>
using namespace std;

// header files in the Boost library: https://www.boost.org/
#include <boost/random.hpp>
boost::random::mt19937 boost_random_time_seed{ static_cast<std::uint32_t>(std::time(0)) };


#include <graph_v_of_v/graph_v_of_v.h>
#include <graph_v_of_v/graph_v_of_v_update_vertexIDs_by_degrees_large_to_small.h>
#include <graph_v_of_v/graph_v_of_v_generate_random_graph.h>
#include <graph_v_of_v/graph_v_of_v_shortest_paths.h>
#include <nonHOP_maintain/nonHOP_maintain_two_hop_labels.h>
#include <nonHOP_maintain/nonHOP_maintain_PLL.h>
#include <text_mining/print_items.h>
#include <text_mining/binary_save_read_vector.h>
#include <nonHOP_maintain/nonHOP_WeightIncreaseMaintenance_improv_batch.h>
#include <nonHOP_maintain/nonHOP_WeightDecreaseMaintenance_improv_batch.h>
#include <nonHOP_maintain/nonHOP_WeightDecrease2021_batch.h>
#include <nonHOP_maintain/nonHOP_WeightIncrease2021_batch.h>




void add_vertex_groups(graph_v_of_v<int>& instance_graph, int group_num) {

	double dummy_edge_probability = 0.2;
	boost::random::uniform_int_distribution<> dist{ static_cast<int>(1), static_cast<int>(100) };

	int N = instance_graph.size();

	instance_graph.ADJs.resize(N + group_num);
	for (int i = N; i < N + group_num; i++) {
		for (int j = 0; j < N; j++) {
			if ((double)dist(boost_random_time_seed) / 100 < dummy_edge_probability) {
				instance_graph.add_edge(i, j, 1e6); // add a dummy edge
			}
		}
	}

}

void test_PLL_check_correctness(two_hop_case_info& case_info, graph_v_of_v<int>& instance_graph, int iteration_source_times, int iteration_terminal_times) {

	/*
	below is for checking whether the above labels are right (by randomly computing shortest paths)

	this function can only be used when 0 to n-1 is in the graph, i.e., the graph is an ideal graph
	*/

	boost::random::uniform_int_distribution<> dist{ static_cast<int>(0), static_cast<int>(instance_graph.ADJs.size() - 1) };

	//graph_hash_of_mixed_weighted_print(instance_graph);

	for (int yy = 0; yy < iteration_source_times; yy++) {
		int source = dist(boost_random_time_seed);

		while (!is_mock[source]) {
			source = dist(boost_random_time_seed);
		}

		std::vector<int> distances, predecessors;

		//source = 0; cout << "source = " << source << endl;

		graph_v_of_v_shortest_paths<int>(instance_graph, source, distances, predecessors);

		for (int xx = 0; xx < iteration_terminal_times; xx++) {

			int terminal = dist(boost_random_time_seed);

			while (is_mock[terminal]) {
				terminal = dist(boost_random_time_seed);
			}

			//terminal = 2; cout << "terminal = " << terminal << endl;

			int dis = extract_distance(case_info, source, terminal);

			if (abs(dis - distances[terminal]) > 1e-4 && (dis < TwoM_value || distances[terminal] < TwoM_value)) {
				cout << "source = " << source << endl;
				cout << "terminal = " << terminal << endl;
				cout << "source vector:" << endl;
				for (auto it = case_info.L[source].begin(); it != case_info.L[source].end(); it++) {
					cout << "<" << it->vertex << "," << it->distance << ">";
				}
				cout << endl;
				cout << "terminal vector:" << endl;
				for (auto it = case_info.L[terminal].begin(); it != case_info.L[terminal].end(); it++) {
					cout << "<" << it->vertex << "," << it->distance << ">";
				}
				cout << endl;

				cout << "query dis = " << dis << endl;
				cout << "distances[terminal] = " << distances[terminal] << endl;
				cout << "abs(dis - distances[terminal]) > 1e-5!" << endl;
				getchar();
			}
		}

	}

}

void graph_change_and_label_maintenance(graph_v_of_v<int>& instance_graph, two_hop_case_info& mm,
	int V, int weightIncrease_time, int weightDecrease_time,  double weightChange_ratio, int thread_num,
	 double& avg_maintain_time, int batch_size, int insert_and_delete_edge, int ec_min, int ec_max, int debug) {

	// cout<<"graph_change_and_label_maintenance"<<endl;
	ThreadPool pool_dynamic(thread_num);
	std::vector<std::future<int>> results_dynamic;
	while (weightIncrease_time + weightDecrease_time) {
		// std::cout<<"change:"<<weightDecrease_time<<" "<<weightIncrease_time<<std::endl;
		if(debug){
			instance_graph.print();
		}
		/*randomly select an edge*/
        vector<pair<int,int> > selected_edge_vec;
        vector<int> selected_edge_weight_vec;
        vector<int> new_edge_weight_vec;
		if(!insert_and_delete_edge){
			int ct=0;
			while (ct<batch_size) {
				boost::random::uniform_int_distribution<> dist_v1{ static_cast<int>(0), static_cast<int>(V - 1) };
				int v1 = dist_v1(boost_random_time_seed);
				if (instance_graph[v1].size() > 0) {
					boost::random::uniform_int_distribution<> dist_v2{ static_cast<int>(0), static_cast<int>(instance_graph[v1].size() - 1) };
					int rand = dist_v2(boost_random_time_seed);
					int v2 = instance_graph[v1][rand].first;
					int wt = instance_graph[v1][rand].second;
					if(is_mock[v2] || wt >= 1e6) continue;
					selected_edge_vec.push_back({ v1,v2 });
					selected_edge_weight_vec.push_back(wt);
					ct++;
				}
			}
		}
		/*change weight*/
		if (weightIncrease_time >= weightDecrease_time) {
			weightIncrease_time--;
            bool next_flag = false;
			if(debug){
				std::cout<<"\nbefore maintain"<<std::endl;
				mm.print_L();
				mm.print_PPR();
				std::cout<<"\nweight change:"<<std::endl;
			}
			if(!insert_and_delete_edge){
				for(int i=0;i<batch_size;i++){
					int new_ec = selected_edge_weight_vec[i] * (1 + weightChange_ratio);
					if (new_ec > 1e6 || new_ec == selected_edge_weight_vec[i]) {
						weightIncrease_time++;
						next_flag = true;
						break;
					}
					new_edge_weight_vec.push_back(new_ec);
					if(debug){
						std::cout<<selected_edge_vec[i].first<<" "<<selected_edge_vec[i].second<<" : "<<selected_edge_weight_vec[i]<<" -> "<<new_ec<<std::endl;
					}
				}
				if(next_flag) continue;
			}
			else{
				int ct=0;
				while (ct<batch_size) {
					boost::random::uniform_int_distribution<> dist_v1{ static_cast<int>(0), static_cast<int>(V - 1) };
					int v1 = dist_v1(boost_random_time_seed);
					if (instance_graph[v1].size() > 0) {
						boost::random::uniform_int_distribution<> dist_v2{ static_cast<int>(0), static_cast<int>(instance_graph[v1].size() - 1) };
						int rand = dist_v2(boost_random_time_seed);
						int v2 = instance_graph[v1][rand].first;
						int wt = instance_graph[v1][rand].second;
						if(is_mock[v2] || wt >= 1e6) continue;
						if(v1>v2) swap(v1,v2);
						if(find(selected_edge_vec.begin(),selected_edge_vec.end(),make_pair(v1,v2)) != selected_edge_vec.end()) continue;
						selected_edge_vec.push_back({ v1,v2 });
						selected_edge_weight_vec.push_back(wt);
						new_edge_weight_vec.push_back(MAX_VALUE);
						if(debug){
							std::cout<<v1<<" "<<v2<<" : "<<wt<<" -> "<<MAX_VALUE<<std::endl;
						}
						ct++;
					}
				}
			}

			for(int i=0;i<batch_size;i++){
				instance_graph.add_edge(selected_edge_vec[i].first,selected_edge_vec[i].second,new_edge_weight_vec[i]);
			}

			auto begin = std::chrono::high_resolution_clock::now();


			/*maintain labels*/
			nonHOP_WeightIncreaseMaintenance_improv_batch(instance_graph, mm, selected_edge_vec, selected_edge_weight_vec, pool_dynamic, results_dynamic);
			//nonHOP_WeightIncrease2021_batch(instance_graph, mm, selected_edge_vec, selected_edge_weight_vec, pool_dynamic, results_dynamic);
			if(debug){
				std::cout<<"\nafter maintain"<<std::endl;
				mm.print_L();
				mm.print_PPR();
				std::cout<<std::endl;
			}

			auto end = std::chrono::high_resolution_clock::now();
			avg_maintain_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s

			// if(insert_and_delete_edge)
			// 	for(auto e:selected_edge_vec){
			// 		instance_graph.remove_edge(e.first,e.second);
			// 	}
		}
		else {
			weightDecrease_time--;
            bool next_flag = false;
			if(debug){
				std::cout<<"\nbefore maintain"<<std::endl;
				mm.print_L();
				mm.print_PPR();
				std::cout<<"\nweight change:"<<std::endl;
			}
			if(!insert_and_delete_edge){
				for(int i=0;i<batch_size;i++){
					weightTYPE new_ec = selected_edge_weight_vec[i] * (1 - weightChange_ratio);
					if(new_ec<1){
						weightDecrease_time++;
						next_flag = true;
						break;
					}
					new_edge_weight_vec.push_back(new_ec);
					if(debug){
						std::cout<<selected_edge_vec[i].first<<" "<<selected_edge_vec[i].second<<" : "<<selected_edge_weight_vec[i]<<" -> "<<new_ec<<std::endl;
					}
				}

				if(next_flag) continue;
			}
			else{
				int ct=0;
				while (ct<batch_size) {
					boost::random::uniform_int_distribution<> dist_v1{ static_cast<int>(0), static_cast<int>(V - 1) };
					int v1 = dist_v1(boost_random_time_seed);
					if (instance_graph[v1].size() < instance_graph.size()) {
						boost::random::uniform_int_distribution<> dist_v2{ static_cast<int>(0), static_cast<int>(V - 1) };
						int v2 = dist_v2(boost_random_time_seed);
						int wt = instance_graph.edge_weight(v1,v2);
						if(is_mock[v2] || wt <= 1e6) continue;
						if(v1>v2) swap(v1,v2);
						if(find(selected_edge_vec.begin(),selected_edge_vec.end(),make_pair(v1,v2)) != selected_edge_vec.end()) continue;
						selected_edge_vec.push_back({ v1,v2 });
						selected_edge_weight_vec.push_back(wt);
						boost::random::uniform_int_distribution<> dist{ static_cast<int>(ec_min), static_cast<int>(ec_max)};
						int new_ec = dist(boost_random_time_seed);
						new_edge_weight_vec.push_back(new_ec);
						if(debug){
							std::cout<<v1<<" "<<v2<<" : "<<wt<<" -> "<<new_ec<<std::endl;
						}
						ct++;
					}
				}
			}

			for(int i=0;i<batch_size;i++){
				instance_graph.add_edge(selected_edge_vec[i].first,selected_edge_vec[i].second,new_edge_weight_vec[i]);
			}

			auto begin = std::chrono::high_resolution_clock::now();



            /*maintain labels*/
			nonHOP_WeightDecreaseMaintenance_improv_batch(instance_graph, mm, selected_edge_vec, new_edge_weight_vec, pool_dynamic, results_dynamic);
			//nonHOP_WeightDecrease2021_batch(instance_graph, mm, selected_edge_vec, new_edge_weight_vec, pool_dynamic, results_dynamic);
			if(debug){
				std::cout<<"\nafter maintain"<<std::endl;
				mm.print_L();
				mm.print_PPR();
				std::cout<<std::endl;
			}

			auto end = std::chrono::high_resolution_clock::now();
			avg_maintain_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
		}

	}
}

void test_PLL() {

	/*parameters*/
	int iteration_graph_times = 2e2, iteration_source_times = 10, iteration_terminal_times = 10;
	int V = 1000, E = 5000, group_num = 10, thread_num = 10;
	int ec_min = 1, ec_max = 20;

	int weightIncrease_time = 50, weightDecrease_time = 50;
	double weightChange_ratio = 0.2;
	int batch_size = 100;

	double avg_index_time = 0, avg_index_size_per_v = 0, avg_maintain_time = 0;
	double avg_canonical_repair_remove_label_ratio = 0;

	int generate_new_graph = 1;

	int insert_and_delete_edge = 1;

	int debug = 0;

	/*reduction method selection*/
	two_hop_case_info mm;
	mm.max_labal_byte_size = 6e9;
	mm.max_run_time_seconds = 1e2;
	mm.use_2M_prune = 1;
	mm.use_rank_prune = 1;
	mm.use_canonical_repair = 0;
	mm.thread_num = 10;

	/*iteration*/
	for (int i = 0; i < iteration_graph_times; i++) {
		cout << "iteration " << i << endl;

		/*input and output; below is for generating random new graph, or read saved graph*/

		graph_v_of_v<int> instance_graph;
		if (generate_new_graph == 1) {
			instance_graph = graph_v_of_v_generate_random_graph<int>(V, E, ec_min, ec_max, 1, boost_random_time_seed);
			/*add vertex groups*/
			if (group_num > 0) {
				add_vertex_groups(instance_graph, group_num);
			}
			for (int j = 0; j < V; j++) {
				is_mock[j] = false;
			}
			for (int j = 0; j < group_num; j++) {
				is_mock[V + j] = true;
			}
			instance_graph = graph_v_of_v_update_vertexIDs_by_degrees_large_to_small_mock(instance_graph, is_mock); // sort vertices
			instance_graph.txt_save("simple_iterative_tests.txt");
			binary_save_vector("simple_iterative_tests_is_mock.txt", is_mock);
		}
		else {
			instance_graph.txt_read("simple_iterative_tests.txt");
			binary_read_vector("simple_iterative_tests_is_mock.txt", is_mock);
		}

		auto begin = std::chrono::high_resolution_clock::now();
		try {
			PLL(instance_graph, mm);
				if (0) {
					instance_graph.print();
					mm.print_L();
					mm.print_PPR();
			}
		}
		catch (string s) {
			cout << s << endl;
			PLL_clear_global_values();
			continue;
		}
		auto end = std::chrono::high_resolution_clock::now();
		double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
		avg_index_time = avg_index_time + runningtime / iteration_graph_times;
		avg_canonical_repair_remove_label_ratio = avg_canonical_repair_remove_label_ratio + mm.canonical_repair_remove_label_ratio / iteration_graph_times;

		initialize_global_values_dynamic(V+group_num, thread_num);
		graph_change_and_label_maintenance(instance_graph, mm, V, weightIncrease_time, weightDecrease_time, weightChange_ratio, thread_num, avg_maintain_time, batch_size, insert_and_delete_edge, ec_min, ec_max, debug);

		/*debug*/
		if (0) {
			instance_graph.print();
			mm.print_L();
		}

		test_PLL_check_correctness(mm, instance_graph, iteration_source_times, iteration_terminal_times);

		long long int index_size = 0;
		for (auto it = mm.L.begin(); it != mm.L.end(); it++) {
			index_size = index_size + (*it).size();
		}
		avg_index_size_per_v = avg_index_size_per_v + (double)index_size / V / iteration_graph_times;

		mm.clear_labels();
	}

	cout << "avg_index_time: " << avg_index_time << "s" << endl;
	cout << "avg_index_size_per_v: " << avg_index_size_per_v << endl;
	cout << "avg_maintain_time: " << (double)avg_maintain_time / iteration_graph_times << "s" << endl;
	cout << "avg_canonical_repair_remove_label_ratio: " << avg_canonical_repair_remove_label_ratio << endl;
}



int main()
{
	test_PLL();
}


