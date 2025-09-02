#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <string>
#include <list>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <typeinfo>
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <math.h>
#include <thread>
#include <chrono>
#include <shared_mutex>
using namespace std;

/*header files in the Boost library: https://www.boost.org/ */
#include <boost/random.hpp>
boost::random::mt19937 boost_random_time_seed{static_cast<std::uint32_t>(std::time(0))};
#include <boost/range/algorithm.hpp>

#include <graph_v_of_v/graph_v_of_v.h>
#include <graph_v_of_v/graph_v_of_v_update_vertexIDs_by_degrees_large_to_small.h>
#include <graph_v_of_v/graph_v_of_v_generate_random_graph.h>
#include <graph_v_of_v/graph_v_of_v_shortest_paths.h>
#include <nonHOP_maintain/nonHOP_maintain_two_hop_labels.h>
#include <nonHOP_maintain/nonHOP_maintain_PLL.h>
#include <text_mining/binary_save_read_vector.h>
#include <nonHOP_maintain/nonHOP_WeightIncreaseMaintenance_improv_batch.h>
#include <nonHOP_maintain/nonHOP_WeightDecreaseMaintenance_improv_batch.h>
#include <nonHOP_maintain/nonHOP_WeightDecrease2021_batch.h>
#include <nonHOP_maintain/nonHOP_WeightIncrease2021_batch.h>

string global_path = "HL-GST_data//";

/*generate_L_PPR*/

void generate_L_PPR()
{

	cout << "press any key to continue generate_L_PPR" << endl;
	getchar();

	//vector<string> used_datas = {"musae", "twitch", "github", "amazon", "dblp", "reddit"};
	vector<string> used_datas = {"liveJournal"};

	/*Jacard & random*/
	for (int i = 0; i < used_datas.size(); i++)
	{

		/*read data*/
		string data_name = used_datas[i];
		string ec_type_name = "Jacard";
		graph_v_of_v<int> input_graph_Mock;
		input_graph_Mock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_Mock_" + ec_type_name + ".binary");

		if (1)
		{
			two_hop_case_info mm;
			mm.use_2M_prune = 1;
			mm.use_rank_prune = 1;
			mm.use_canonical_repair = 0;
			mm.thread_num = 80;

			PLL(input_graph_Mock, mm);

			string path = "HL-GST_data//";
			binary_save_PPR(path + data_name + "_PPR_2M.bin", mm.PPR);
			binary_save_vector_of_vectors(path + data_name + "_L_2M.bin", mm.L);
		}

		if (1)
		{
			two_hop_case_info mm;
			mm.use_2M_prune = 0;
			mm.use_rank_prune = 1;
			mm.use_canonical_repair = 0;
			mm.thread_num = 80;

			PLL(input_graph_Mock, mm);

			string path = "HL-GST_data//";
			binary_save_PPR(path + data_name + "_PPR.bin", mm.PPR);
			binary_save_vector_of_vectors(path + data_name + "_L.bin", mm.L);
		}
	}
}

/*exp*/

class edge_change
{
public:
	int v1, v2, old_weight, new_weight;
};

pair<vector<edge_change>, vector<edge_change>> select_decrease_increase_weights(graph_v_of_v<int> &input_graph, int batch_N)
{

	vector<edge_change> decrease_edges, increase_edges;

	int V = input_graph.size();

	/*decrease_edges*/
	if (1)
	{ // add mock edges
		vector<int> non_mocks, mocks;
		for (int i = 0; i < V; i++)
		{
			if (is_mock[i])
			{
				mocks.push_back(i);
			}
			else
			{
				non_mocks.push_back(i);
			}
		}
		int left_change_times = batch_N;
		boost::random::uniform_int_distribution<> dist1{static_cast<int>(0), static_cast<int>(mocks.size() - 1)};
		boost::random::uniform_int_distribution<> dist2{static_cast<int>(0), static_cast<int>(non_mocks.size() - 1)};
		while (left_change_times)
		{
			edge_change x;
			x.v1 = mocks[dist1(boost_random_time_seed)];
			x.v2 = non_mocks[dist2(boost_random_time_seed)];
			x.old_weight = input_graph.edge_weight(x.v1, x.v2);
			x.new_weight = 1e6;
			decrease_edges.push_back(x);
			left_change_times--;
		}
	}
	if (0)
	{
		vector<pair<int, int>> edge_pool;
		for (int i = 0; i < V; i++)
		{
			for (auto adj : input_graph[i])
			{
				if (i < adj.first && input_graph[i].size())
				{
					edge_pool.push_back({i, adj.first});
				}
			}
		}
		boost::range::random_shuffle(edge_pool);
		boost::random::uniform_int_distribution<> dist{static_cast<int>(0), static_cast<int>(edge_pool.size() - 1)};
		int left_change_times = batch_N;
		while (left_change_times)
		{
			edge_change x;

			int r = dist(boost_random_time_seed);
			x.v1 = edge_pool[r].first;
			x.v2 = edge_pool[r].second;

			// x.v1 = edge_pool.back().first;
			// x.v2 = edge_pool.back().second;
			x.old_weight = input_graph.edge_weight(x.v1, x.v2);
			x.new_weight = x.old_weight;
			decrease_edges.push_back(x);
			// edge_pool.pop_back();
			left_change_times--;
		}
	}

	/*increase_edges*/
	if (1)
	{ // randomly deleting sparse mock and non-mock edges
		vector<pair<int, int>> edge_pool;
		for (int i = 0; i < V; i++)
		{
			for (auto adj : input_graph[i])
			{
				if (i < adj.first && min(input_graph[i].size(), input_graph[adj.first].size()) < 5)
				{ // 10 makes 2023algo too slow
					edge_pool.push_back({i, adj.first});
				}
			}
		}
		boost::range::random_shuffle(edge_pool);
		boost::random::uniform_int_distribution<> dist{static_cast<int>(0), static_cast<int>(edge_pool.size() - 1)};
		int left_change_times = batch_N;
		while (left_change_times)
		{
			edge_change x;
			int r = dist(boost_random_time_seed);
			x.v1 = edge_pool[r].first;
			x.v2 = edge_pool[r].second;
			x.old_weight = input_graph.edge_weight(x.v1, x.v2);
			x.new_weight = 1e7;
			increase_edges.push_back(x);
			left_change_times--;
		}
	}
	if (0)
	{ // only removing mock edges is extremely fast (no query in maintenance is needed)
		vector<int> non_mocks, mocks;
		for (int i = 0; i < V; i++)
		{
			if (is_mock[i])
			{
				mocks.push_back(i);
			}
			else
			{
				non_mocks.push_back(i);
			}
		}
		int left_change_times = batch_N;
		boost::random::uniform_int_distribution<> dist1{static_cast<int>(0), static_cast<int>(mocks.size() - 1)};
		while (left_change_times)
		{

			int v1 = mocks[dist1(boost_random_time_seed)];
			if (input_graph[v1].size() > 0)
			{
				boost::random::uniform_int_distribution<> dist2{static_cast<int>(0), static_cast<int>(input_graph[v1].size() - 1)};
				int v2 = input_graph[v1][dist2(boost_random_time_seed)].first;
				edge_change x;
				x.v1 = v1;
				x.v2 = v2;
				x.old_weight = input_graph.edge_weight(x.v1, x.v2);
				x.new_weight = 1e7;
				decrease_edges.push_back(x);
				left_change_times--;
			}
		}
	}

	return {decrease_edges, increase_edges};
}

void exp_element_nonHOP(string data_name, string file_name, int iteration_num)
{

	/*read data*/
	string ec_type_name = "Jacard";
	graph_v_of_v<int> input_graph_Mock_initial;
	input_graph_Mock_initial.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_Mock_" + ec_type_name + ".binary");
	vector<int> new_is_mock;
	binary_read_vector(global_path + data_name + "//exp_" + data_name + "_new_is_mock_" + ec_type_name + ".binary", new_is_mock);
	is_mock = new_is_mock; // update is_mock
	string path = "HL-GST_data//";
	two_hop_case_info mm_initial_2M, mm_initial;
	binary_read_PPR(path + data_name + "_PPR_2M.bin", mm_initial_2M.PPR);
	binary_read_vector_of_vectors(path + data_name + "_L_2M.bin", mm_initial_2M.L);
	binary_read_PPR(path + data_name + "_PPR.bin", mm_initial.PPR);
	binary_read_vector_of_vectors(path + data_name + "_L.bin", mm_initial.L);

	int thread_num = 80;
	vector<int> batches = {1, 10, 20};

	/*output*/
	ofstream outputFile;
	outputFile.precision(8);
	outputFile.setf(ios::fixed);
	outputFile.setf(ios::showpoint);
	string save_name = "expHL4GST_nonHOP_maintain_" + file_name + "_" + ec_type_name + ".csv";
	cout << "start indexing " << save_name << endl;
	outputFile.open(save_name);

	outputFile << "batch1,new_decrease_time1(s),new_decrease_query,new_decrease_Loperation,new_increase_time1,new_increase_query,new_increase_Loperation,"
			   << "2023_decrease_time1,_2023_decrease_query,_2023_decrease_query_NOless_2M,_2023_decrease_Loperation,_2023_decrease_Loperation_NOless_2M,"
			   << "2023_increase_time1,_2023_increase_query,_2023_increase_query_NOless_2M,_2023_increase_Loperation,_2023_increase_Loperation_NOless_2M,"
			   << "batch2,new_decrease_time1(s),new_decrease_query,new_decrease_Loperation,new_increase_time1,new_increase_query,new_increase_Loperation,"
			   << "2023_decrease_time1,_2023_decrease_query,_2023_decrease_query_NOless_2M,_2023_decrease_Loperation,_2023_decrease_Loperation_NOless_2M,"
			   << "2023_increase_time1,_2023_increase_query,_2023_increase_query_NOless_2M,_2023_increase_Loperation,_2023_increase_Loperation_NOless_2M,"
			   << "batch3,new_decrease_time1(s),new_decrease_query,new_decrease_Loperation,new_increase_time1,new_increase_query,new_increase_Loperation,"
			   << "2023_decrease_time1,_2023_decrease_query,_2023_decrease_query_NOless_2M,_2023_decrease_Loperation,_2023_decrease_Loperation_NOless_2M,"
			   << "2023_increase_time1,_2023_increase_query,_2023_increase_query_NOless_2M,_2023_increase_Loperation,_2023_increase_Loperation_NOless_2M,"
			   << "d1new_decrease_time1,d1new_increase_time1,d12023_decrease_time1,d12023_increase_time1,d40new_decrease_time1,d40new_increase_time1,d402023_decrease_time1,d402023_increase_time1,irregular_state,min_accelerate_ratio" << endl;

	vector<vector<double>> new_decrease_time(3), new_increase_time(3), _2023_decrease_time(3), _2023_increase_time(3),
		new_decrease_query_times(3), new_increase_query_times(3), _2023_decrease_query_times(3), _2023_increase_query_times(3), _2023_decrease_query_times_NOless_2M(3), _2023_increase_query_times_NOless_2M(3),
		new_decrease_Loperation_times(3), new_increase_Loperation_times(3), _2023_decrease_Loperation_times(3), _2023_increase_Loperation_times(3), _2023_decrease_Loperation_times_NOless_2M(3), _2023_increase_Loperation_times_NOless_2M(3);

	for (int i = 0; i < iteration_num; i++)
	{

		initialize_global_values_dynamic(input_graph_Mock_initial.size() + 1, thread_num);

		double d1new_decrease_time, d1new_increase_time, d12023_decrease_time, d12023_increase_time, d40new_decrease_time, d40new_increase_time, d402023_decrease_time, d402023_increase_time;

		int ii = -1;
		for (auto batch_N : batches)
		{

			cout << "batches " << batch_N << endl;

			ii++;
			pair<vector<edge_change>, vector<edge_change>> changes = select_decrease_increase_weights(input_graph_Mock_initial, batch_N);
			graph_v_of_v<int> new_graph_decrease = input_graph_Mock_initial, new_graph_increase = input_graph_Mock_initial;
			vector<pair<int, int>> selected_edge_decrease, selected_edge_increase;
			vector<int> selected_edge_old_weight_decrease, selected_edge_new_weight_decrease, selected_edge_old_weight_increase, selected_edge_new_weight_increase;
			for (auto x : changes.first)
			{
				new_graph_decrease.add_edge(x.v1, x.v2, x.new_weight);
				selected_edge_decrease.push_back({x.v1, x.v2});
				selected_edge_old_weight_decrease.push_back(x.old_weight);
				selected_edge_new_weight_decrease.push_back(x.new_weight);
			}
			for (auto x : changes.second)
			{
				new_graph_increase.add_edge(x.v1, x.v2, x.new_weight);
				selected_edge_increase.push_back({x.v1, x.v2});
				selected_edge_old_weight_increase.push_back(x.old_weight);
				selected_edge_new_weight_increase.push_back(x.new_weight);
			}

			/*new decrease*/
			if (1)
			{
				cout << "new decrease" << endl;
				auto mm = mm_initial_2M;
				ThreadPool pool_dynamic(thread_num);
				std::vector<std::future<int>> results_dynamic;
				auto begin = std::chrono::high_resolution_clock::now();
				nonHOP_WeightDecreaseMaintenance_improv_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
				auto end = std::chrono::high_resolution_clock::now();
				double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
				new_decrease_time[ii].push_back(t);
				new_decrease_query_times[ii].push_back(global_query_times);
				new_decrease_Loperation_times[ii].push_back(label_operation_times);
				cout << "new decrease end" << endl;
			}
			else
			{
				new_decrease_time[ii].push_back(0);
				new_decrease_query_times[ii].push_back(0);
				new_decrease_Loperation_times[ii].push_back(0);
			}

			/*new increase*/
			if (1)
			{
				cout << "new increase" << endl;
				auto mm = mm_initial_2M;
				ThreadPool pool_dynamic(thread_num);
				std::vector<std::future<int>> results_dynamic;
				auto begin = std::chrono::high_resolution_clock::now();
				nonHOP_WeightIncreaseMaintenance_improv_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
				auto end = std::chrono::high_resolution_clock::now();
				double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
				new_increase_time[ii].push_back(t);
				new_increase_query_times[ii].push_back(global_query_times);
				new_increase_Loperation_times[ii].push_back(label_operation_times);
				cout << "new increase end" << endl;
			}
			else
			{
				new_increase_time[ii].push_back(0);
				new_increase_query_times[ii].push_back(0);
				new_increase_Loperation_times[ii].push_back(0);
			}

			/*2023 decrease*/
			if (1)
			{
				cout << "2023 decrease" << endl;
				auto mm = mm_initial;
				ThreadPool pool_dynamic(thread_num);
				std::vector<std::future<int>> results_dynamic;
				try
				{
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightDecrease2021_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					_2023_decrease_time[ii].push_back(t);
					_2023_decrease_query_times[ii].push_back(global_query_times);
					_2023_decrease_Loperation_times[ii].push_back(label_operation_times);
					_2023_decrease_query_times_NOless_2M[ii].push_back(global_query_times_NOless_2M);
					_2023_decrease_Loperation_times_NOless_2M[ii].push_back(label_operation_times_NOless_2M);
				}
				catch (string s)
				{
					_2023_decrease_time[ii].push_back(-1);
					_2023_decrease_query_times[ii].push_back(global_query_times);
					_2023_decrease_Loperation_times[ii].push_back(label_operation_times);
					_2023_decrease_query_times_NOless_2M[ii].push_back(global_query_times_NOless_2M);
					_2023_decrease_Loperation_times_NOless_2M[ii].push_back(label_operation_times_NOless_2M);
				}
				cout << "2023 decrease end" << endl;
			}
			else
			{
				_2023_decrease_time[ii].push_back(0);
				_2023_decrease_query_times[ii].push_back(0);
				_2023_decrease_Loperation_times[ii].push_back(0);
				_2023_decrease_query_times_NOless_2M[ii].push_back(0);
				_2023_decrease_Loperation_times_NOless_2M[ii].push_back(0);
			}

			/*2023 increase*/
			if (1)
			{
				cout << "2023 increase" << endl;
				auto mm = mm_initial;
				ThreadPool pool_dynamic(thread_num);
				std::vector<std::future<int>> results_dynamic;
				try
				{
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightIncrease2021_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					_2023_increase_time[ii].push_back(t);
					_2023_increase_query_times[ii].push_back(global_query_times);
					_2023_increase_Loperation_times[ii].push_back(label_operation_times);
					_2023_increase_query_times_NOless_2M[ii].push_back(global_query_times_NOless_2M);
					_2023_increase_Loperation_times_NOless_2M[ii].push_back(label_operation_times_NOless_2M);
				}
				catch (string s)
				{
					_2023_increase_time[ii].push_back(-1);
					_2023_increase_query_times[ii].push_back(global_query_times);
					_2023_increase_Loperation_times[ii].push_back(label_operation_times);
					_2023_increase_query_times_NOless_2M[ii].push_back(global_query_times_NOless_2M);
					_2023_increase_Loperation_times_NOless_2M[ii].push_back(label_operation_times_NOless_2M);
				}
				cout << "2023 increase end" << endl;
			}
			else
			{
				_2023_increase_time[ii].push_back(0);
				_2023_increase_query_times[ii].push_back(0);
				_2023_increase_Loperation_times[ii].push_back(0);
				_2023_increase_query_times_NOless_2M[ii].push_back(0);
				_2023_increase_Loperation_times_NOless_2M[ii].push_back(0);
			}

			if (batch_N == batches[2])
			{
				/*new decrease d1*/
				if (1)
				{
					cout << "new decrease d1" << endl;
					auto mm = mm_initial_2M;
					ThreadPool pool_dynamic(1);
					std::vector<std::future<int>> results_dynamic;
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightDecreaseMaintenance_improv_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					d1new_decrease_time = t;
					cout << "new decrease end d1" << endl;
				}

				/*new increase d1*/
				if (1)
				{
					cout << "new increase d1" << endl;
					auto mm = mm_initial_2M;
					ThreadPool pool_dynamic(1);
					std::vector<std::future<int>> results_dynamic;
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightIncreaseMaintenance_improv_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					d1new_increase_time = t;
					cout << "new increase end d1" << endl;
				}

				/*2023 decrease d1*/
				if (1)
				{
					cout << "2023 decrease d1" << endl;
					auto mm = mm_initial;
					ThreadPool pool_dynamic(1);
					std::vector<std::future<int>> results_dynamic;
					try
					{
						auto begin = std::chrono::high_resolution_clock::now();
						nonHOP_WeightDecrease2021_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
						auto end = std::chrono::high_resolution_clock::now();
						double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
						d12023_decrease_time = t;
					}
					catch (string s)
					{
						d12023_decrease_time = -1;
					}
					cout << "2023 decrease end d1" << endl;
				}

				/*2023 increase d1*/
				if (1)
				{
					cout << "2023 increase d1" << endl;
					auto mm = mm_initial;
					ThreadPool pool_dynamic(1);
					std::vector<std::future<int>> results_dynamic;
					try
					{
						auto begin = std::chrono::high_resolution_clock::now();
						nonHOP_WeightIncrease2021_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
						auto end = std::chrono::high_resolution_clock::now();
						double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
						d12023_increase_time = t;
					}
					catch (string s)
					{
						d12023_increase_time = -1;
					}
					cout << "2023 increase end d1" << endl;
				}

				/*new decrease d40*/
				if (1)
				{
					cout << "new decrease d40" << endl;
					auto mm = mm_initial_2M;
					ThreadPool pool_dynamic(40);
					std::vector<std::future<int>> results_dynamic;
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightDecreaseMaintenance_improv_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					d40new_decrease_time = t;
					cout << "new decrease end d40" << endl;
				}

				/*new increase d40*/
				if (1)
				{
					cout << "new increase d40" << endl;
					auto mm = mm_initial_2M;
					ThreadPool pool_dynamic(40);
					std::vector<std::future<int>> results_dynamic;
					auto begin = std::chrono::high_resolution_clock::now();
					nonHOP_WeightIncreaseMaintenance_improv_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
					auto end = std::chrono::high_resolution_clock::now();
					double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
					d40new_increase_time = t;
					cout << "new increase end d40" << endl;
				}

				/*2023 decrease d40*/
				if (1)
				{
					cout << "2023 decrease d40" << endl;
					auto mm = mm_initial;
					ThreadPool pool_dynamic(40);
					std::vector<std::future<int>> results_dynamic;
					try
					{
						auto begin = std::chrono::high_resolution_clock::now();
						nonHOP_WeightDecrease2021_batch(new_graph_decrease, mm, selected_edge_decrease, selected_edge_new_weight_decrease, pool_dynamic, results_dynamic);
						auto end = std::chrono::high_resolution_clock::now();
						double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
						d402023_decrease_time = t;
					}
					catch (string s)
					{
						d402023_decrease_time = -1;
					}
					cout << "2023 decrease end d40" << endl;
				}

				/*2023 increase d40*/
				if (1)
				{
					cout << "2023 increase d40" << endl;
					auto mm = mm_initial;
					ThreadPool pool_dynamic(40);
					std::vector<std::future<int>> results_dynamic;
					try
					{
						auto begin = std::chrono::high_resolution_clock::now();
						nonHOP_WeightIncrease2021_batch(new_graph_increase, mm, selected_edge_increase, selected_edge_old_weight_increase, pool_dynamic, results_dynamic);
						auto end = std::chrono::high_resolution_clock::now();
						double t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
						d402023_increase_time = t;
					}
					catch (string s)
					{
						d402023_increase_time = -1;
					}
					cout << "2023 increase end d40" << endl;
				}
			}
		}

		double min_accelerate_ratio = INT_MAX;
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_decrease_time[0][i] / new_decrease_time[0][i]);
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_increase_time[0][i] / new_increase_time[0][i]);
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_decrease_time[1][i] / new_decrease_time[1][i]);
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_increase_time[1][i] / new_increase_time[1][i]);
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_decrease_time[2][i] / new_decrease_time[2][i]);
		min_accelerate_ratio = std::min(min_accelerate_ratio, _2023_increase_time[2][i] / new_increase_time[2][i]);

		long long int irregular_state = 0;
		if (min_accelerate_ratio < 1)
		{
			irregular_state += 1;
		}
		irregular_state += 10 * ((new_decrease_Loperation_times[2][i] >= _2023_decrease_Loperation_times[2][i] - _2023_decrease_Loperation_times_NOless_2M[2][i]) +
								 (new_decrease_query_times[2][i] >= _2023_decrease_query_times[2][i] - _2023_decrease_query_times_NOless_2M[2][i]) +
								 (new_increase_Loperation_times[2][i] >= _2023_increase_Loperation_times[2][i] - _2023_increase_Loperation_times_NOless_2M[2][i]) +
								 (new_increase_query_times[2][i] >= _2023_increase_query_times[2][i] - _2023_increase_query_times_NOless_2M[2][i]));

		irregular_state += 100 * ((d1new_decrease_time < new_decrease_time[2][i]) +
								  (d1new_increase_time < new_increase_time[2][i]) +
								  (d12023_decrease_time < _2023_decrease_time[2][i]) +
								  (d12023_increase_time < _2023_increase_time[2][i]));

		outputFile << batches[0] << "," << new_decrease_time[0][i] << "," << new_decrease_query_times[0][i]
				   << "," << new_decrease_Loperation_times[0][i] << "," << new_increase_time[0][i] << "," << new_increase_query_times[0][i] << "," << new_increase_Loperation_times[0][i]
				   << "," << _2023_decrease_time[0][i] << "," << _2023_decrease_query_times[0][i] << "," << _2023_decrease_query_times_NOless_2M[0][i] << "," << _2023_decrease_Loperation_times[0][i]
				   << "," << _2023_decrease_Loperation_times_NOless_2M[0][i] << "," << _2023_increase_time[0][i] << "," << _2023_increase_query_times[0][i] << "," << _2023_increase_query_times_NOless_2M[0][i]
				   << "," << _2023_increase_Loperation_times[0][i] << "," << _2023_increase_Loperation_times_NOless_2M[0][i] << ",";

		outputFile << batches[1] << "," << new_decrease_time[1][i] << "," << new_decrease_query_times[1][i] << "," << new_decrease_Loperation_times[1][i] << "," << new_increase_time[1][i]
				   << "," << new_increase_query_times[1][i] << "," << new_increase_Loperation_times[1][i] << "," << _2023_decrease_time[1][i] << "," << _2023_decrease_query_times[1][i] << "," << _2023_decrease_query_times_NOless_2M[1][i]
				   << "," << _2023_decrease_Loperation_times[1][i] << "," << _2023_decrease_Loperation_times_NOless_2M[1][i] << "," << _2023_increase_time[1][i] << "," << _2023_increase_query_times[1][i] << "," << _2023_increase_query_times_NOless_2M[1][i]
				   << "," << _2023_increase_Loperation_times[1][i] << "," << _2023_increase_Loperation_times_NOless_2M[1][i] << ",";

		outputFile << batches[2] << "," << new_decrease_time[2][i] << "," << new_decrease_query_times[2][i] << "," << new_decrease_Loperation_times[2][i] << "," << new_increase_time[2][i] << "," << new_increase_query_times[2][i]
				   << "," << new_increase_Loperation_times[2][i] << "," << _2023_decrease_time[2][i] << "," << _2023_decrease_query_times[2][i] << "," << _2023_decrease_query_times_NOless_2M[2][i] << "," << _2023_decrease_Loperation_times[2][i]
				   << "," << _2023_decrease_Loperation_times_NOless_2M[2][i] << "," << _2023_increase_time[2][i] << "," << _2023_increase_query_times[2][i] << "," << _2023_increase_query_times_NOless_2M[2][i] << "," << _2023_increase_Loperation_times[2][i]
				   << "," << _2023_increase_Loperation_times_NOless_2M[2][i] << ",";

		outputFile << d1new_decrease_time << "," << d1new_increase_time << "," << d12023_decrease_time << "," << d12023_increase_time
				   << "," << d40new_decrease_time << "," << d40new_increase_time << "," << d402023_decrease_time << "," << d402023_increase_time << "," << irregular_state << "," << min_accelerate_ratio << endl;
	}

	outputFile.close();
}

void main_exp()
{

	vector<string> used_datas = {"musae", "twitch", "github", "amazon", "dblp", "reddit"};

	/*Jacard & random*/
	if (1)
	{
		for (int i = 0; i < used_datas.size(); i++)
		{
			exp_element_nonHOP(used_datas[i], used_datas[i], 300);
		}
	}
}

int main(int argc, char *argv[])
{
	cout << "Start running..." << endl;
	auto begin = std::chrono::high_resolution_clock::now();
	srand(time(NULL)); //  seed random number generator

	if (argc == 5)
	{ // argv[0] is the name of the exe file  e.g.: ./A musae musae 50 3600
		_2023algo_max_second = atoi(argv[4]);
		exp_element_nonHOP(argv[1], argv[2], atoi(argv[3]));
	}
	else
	{
		generate_L_PPR();
	}

	auto end = std::chrono::high_resolution_clock::now();
	double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	cout << "END    runningtime: " << runningtime << "s" << endl;
}
