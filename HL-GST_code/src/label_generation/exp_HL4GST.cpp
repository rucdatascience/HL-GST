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
boost::random::mt19937 boost_random_time_seed{ static_cast<std::uint32_t>(std::time(0)) };
#include <boost/range/algorithm.hpp>

#include <graph_v_of_v/graph_v_of_v.h>
#include <graph_v_of_v/graph_v_of_v_update_vertexIDs_by_degrees_large_to_small.h>
#include <graph_v_of_v/graph_v_of_v_generate_random_graph.h>
#include <graph_v_of_v/graph_v_of_v_shortest_paths.h>
#include <label_generation/two_hop_labels.h>
#include <label_generation/PLL.h>
#include <label_generation/CT.h>
#include <label_generation/hop_constrained_two_hop_labels_generation.h>
#include <label_generation/GST.h>
#include <text_mining/print_items.h>
#include <text_mining/binary_save_read_vector.h>

string global_path = "HL-GST_data//";

/*exp*/

template <typename case_info_type>
double GST_nonHOP_query(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& GST_query_list, bool has_mock) {

	auto begin = std::chrono::high_resolution_clock::now();
	if (has_mock) {
		auto x = GST_nonHOP(case_info, instance_graph, GST_query_list);
	}
	else {
		auto x = GST_nonHOP_noMOCK(case_info, instance_graph, GST_query_list);
	}

	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9; // s
}

template <typename case_info_type>
double GST_HOP_query(case_info_type& case_info, graph_v_of_v<int>& instance_graph, vector<int>& GST_query_list, int diameter_cst, bool has_mock) {

	auto begin = std::chrono::high_resolution_clock::now();
	if (has_mock) {
		auto x = GST_HOP(case_info, instance_graph, GST_query_list, diameter_cst);
	}
	else {
		auto x = GST_HOP_noMOCK(case_info, instance_graph, GST_query_list, diameter_cst);
	}

	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1e9; // s
}

void exp_element_nonHOP_static(string data_name, int ec_type, int thread_num, long long int max_byte_size, double max_run_time_seconds) {

	/*read data*/
	string ec_type_name;
	if (ec_type == 0) {
		ec_type_name = "Jacard";
	}
	else if (ec_type == 1) {
		ec_type_name = "random";
	}
	graph_v_of_v<int> input_graph_Mock, input_graph_noMock;
	vector<int> new_is_mock;
	vector<vector<int>> select_groups1, select_groups2, select_groups3;
	input_graph_Mock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_Mock_" + ec_type_name + ".binary");
	input_graph_noMock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_noMock_" + ec_type_name + ".binary");
	binary_read_vector(global_path + data_name + "//exp_" + data_name + "_new_is_mock_" + ec_type_name + ".binary", new_is_mock);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups1_" + ec_type_name + ".binary", select_groups1);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups2_" + ec_type_name + ".binary", select_groups2);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups3_" + ec_type_name + ".binary", select_groups3);
	is_mock = new_is_mock; // update is_mock
	int query_times = select_groups3.size();

	/*parameters*/
	int d = 20;

	/*output*/
	ofstream outputFile;
	outputFile.precision(8);
	outputFile.setf(ios::fixed);
	outputFile.setf(ios::showpoint);
	string save_name = "expHL4GST_nonHOP_" + data_name + "_" + ec_type_name + "_thread_num_" + to_string(thread_num) + ".csv";
	cout << "start indexing " << save_name << endl;
	outputFile.open(save_name);

	outputFile << "WWW2020NoMock_index_time,WWW2020NoMock_index_size(byte),WWW2020NoMock_GST_time1,WWW2020NoMock_GST_time2,WWW2020NoMock_GST_time3,"
		<< "WWW2020WithMock_index_time,WWW2020WithMock_index_size(byte),WWW2020WithMock_GST_time1,WWW2020Mock_avgMockL_size1,WWW2020WithMock_GST_time2,WWW2020Mock_avgMockL_size2,WWW2020WithMock_GST_time3,WWW2020Mock_avgMockL_size3,"
		<< "CT_index_time,CT_index_size(byte),CT_GST_time1,CT_avgMockL_size1,CT_GST_time2,CT_avgMockL_size2,CT_GST_time3,CT_avgMockL_size3,"
		<< "newHL_index_time,newHL_index_size(byte),newHL_GST_time1,newHL_avgMockL_size1,newHL_GST_time2,newHL_avgMockL_size2,newHL_GST_time3,newHL_avgMockL_size3,"
		<< "WWW2020_newHL_Qratio,NOINDEX_GST_time1,NOINDEX_GST_time2,NOINDEX_GST_time3," << endl;

	double WWW2020NoMock_index_time, WWW2020NoMock_index_size, WWW2020WithMock_index_time, WWW2020WithMock_index_size, CT_index_time, CT_index_size, newHL_index_time, newHL_index_size;

	vector<double> WWW2020NoMock_GST_time1, WWW2020NoMock_GST_time2, WWW2020NoMock_GST_time3,
		WWW2020WithMock_GST_time1, WWW2020WithMock_GST_time2, WWW2020WithMock_GST_time3, WWW2020Mock_avgMockL_size1, WWW2020Mock_avgMockL_size2, WWW2020Mock_avgMockL_size3,
		CT_GST_time1, CT_GST_time2, CT_GST_time3, CT_avgMockL_size1, CT_avgMockL_size2, CT_avgMockL_size3,
		newHL_GST_time1, newHL_GST_time2, newHL_GST_time3, newHL_avgMockL_size1, newHL_avgMockL_size2, newHL_avgMockL_size3,
		WWW2020_newHL_Qratio,
		NOINDEX_GST_time1, NOINDEX_GST_time2, NOINDEX_GST_time3;

	
	cout<< "step 1" <<endl;
	/* WWW2020NoMock */
	if (0) {
		string algo_name = "WWW2020NoMock";
		cout << "start " + algo_name << endl;

		two_hop_case_info mm;
		mm.thread_num = thread_num;
		mm.max_labal_byte_size = max_byte_size;
		mm.max_run_time_seconds = max_run_time_seconds;
		mm.use_rank_prune = 0;
		mm.use_2M_prune = 0;
		mm.use_canonical_repair = 0;

		bool catch_error = false;
		try {
			PLL(input_graph_noMock, mm); // no mock
		}
		catch (string s) {
			catch_error = true;
			cout << s << endl;
			PLL_clear_global_values();
		}
		mm.record_all_details(save_name + "_" + algo_name);
		cout << "finish " + algo_name << endl;

		if (!catch_error) {
			WWW2020NoMock_index_time = mm.time_total, WWW2020NoMock_index_size = mm.compute_label_byte_size();
			ThreadPool pool(50);
			std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
			for (int i = 0; i < query_times; i++) {
				results.emplace_back(
					pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i] { // pass const type value j to thread; [] can be empty

						double xx1, xx2, xx3;
						try {
							GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], false); // this is to warm up this thread
							xx1 = GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], false);
						}
						catch (string s) {
							xx1 = INT_MAX;
						}
						try {
							xx2 = GST_nonHOP_query(mm, input_graph_Mock, select_groups2[i], false);
						}
						catch (string s) {
							xx2 = INT_MAX;
						}
						try {
							xx3 = GST_nonHOP_query(mm, input_graph_Mock, select_groups3[i], false);
						}
						catch (string s) {
							xx3 = INT_MAX;
						}

						tuple<double, double, double> x = { xx1, xx2, xx3 };
						return x; // return to results; the return type must be the same with results
						})
				);
			}
			for (auto&& result : results) {
				auto xx = result.get(); //all threads finish here
				WWW2020NoMock_GST_time1.push_back(std::get<0>(xx)), WWW2020NoMock_GST_time2.push_back(std::get<1>(xx)), WWW2020NoMock_GST_time3.push_back(std::get<2>(xx));
			}
		}
		else {
			WWW2020NoMock_index_time = -1, WWW2020NoMock_index_size = -1;
			for (int i = 0; i < query_times; i++) {
				WWW2020NoMock_GST_time1.push_back(-1), WWW2020NoMock_GST_time2.push_back(-1), WWW2020NoMock_GST_time3.push_back(-1);
			}
		}
	}

	cout<< "step 2" <<endl;
	/* WWW2020WithMock */
	if (0) {
		string algo_name = "WWW2020WithMock";
		cout << "start " + algo_name << endl;

		two_hop_case_info mm;
		mm.thread_num = thread_num;
		mm.max_labal_byte_size = max_byte_size;
		mm.max_run_time_seconds = max_run_time_seconds;
		mm.use_rank_prune = 0;
		mm.use_2M_prune = 0;
		mm.use_canonical_repair = 0;

		bool catch_error = false;
		try {
			PLL(input_graph_Mock, mm); // with mock
		}
		catch (string s) {
			catch_error = true;
			cout << s << endl;
			PLL_clear_global_values();
		}
		mm.record_all_details(save_name + "_" + algo_name);
		cout << "finish " + algo_name << endl;

		for (int i = 0; i < query_times; i++) {
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups1[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups1[i].size();
				WWW2020Mock_avgMockL_size1.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups2[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups2[i].size();
				WWW2020Mock_avgMockL_size2.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups3[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups3[i].size();
				WWW2020Mock_avgMockL_size3.push_back(avgMockL_size);
			}
		}

		if (!catch_error) {
			WWW2020WithMock_index_time = mm.time_total, WWW2020WithMock_index_size = mm.compute_label_byte_size();
			ThreadPool pool(50);
			std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
			for (int i = 0; i < query_times; i++) {
				results.emplace_back(
					pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i] { // pass const type value j to thread; [] can be empty

						GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true); // this is to warm up this thread
						double xx1 = GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true);
						double xx2 = GST_nonHOP_query(mm, input_graph_Mock, select_groups2[i], true);
						double xx3 = GST_nonHOP_query(mm, input_graph_Mock, select_groups3[i], true);

						tuple<double, double, double> x = { xx1, xx2, xx3 };
						return x; // return to results; the return type must be the same with results
						})
				);
			}
			for (auto&& result : results) {
				auto xx = result.get(); //all threads finish here
				WWW2020WithMock_GST_time1.push_back(std::get<0>(xx)), WWW2020WithMock_GST_time2.push_back(std::get<1>(xx)), WWW2020WithMock_GST_time3.push_back(std::get<2>(xx));
			}
		}
		else {
			WWW2020WithMock_index_time = -1, WWW2020WithMock_index_size = -1;
			for (int i = 0; i < query_times; i++) {
				WWW2020WithMock_GST_time1.push_back(-1), WWW2020WithMock_GST_time2.push_back(-1), WWW2020WithMock_GST_time3.push_back(-1);
			}
		}
	}

	cout<< "step 3" <<endl;
	/* newHL */
	if (1) {
		string algo_name = "newHL";
		cout << "start " + algo_name << endl;

		two_hop_case_info mm;
		mm.thread_num = thread_num;
		mm.max_labal_byte_size = max_byte_size;
		mm.max_run_time_seconds = max_run_time_seconds;
		mm.use_rank_prune = 1;
		mm.use_2M_prune = 1;
		mm.use_canonical_repair = 1;

		bool catch_error = false;
		try {
			PLL(input_graph_Mock, mm); // with mock
		}
		catch (string s) {
			catch_error = true;
			cout << s << endl;
			PLL_clear_global_values();
		}
		mm.record_all_details(save_name + "_" + algo_name);
		cout << "finish " + algo_name << endl;

		for (int i = 0; i < query_times; i++) {
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups1[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups1[i].size();
				newHL_avgMockL_size1.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups2[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups2[i].size();
				newHL_avgMockL_size2.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups3[i]) {
					avgMockL_size += mm.L[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups3[i].size();
				newHL_avgMockL_size3.push_back(avgMockL_size);
			}
		}

		if (!catch_error) {
			newHL_index_time = mm.time_total, newHL_index_size = mm.compute_label_byte_size();
			ThreadPool pool(50);
			std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
			for (int i = 0; i < query_times; i++) {
				results.emplace_back(
					pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i] { // pass const type value j to thread; [] can be empty

						GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true); // this is to warm up this thread
						double xx1 = GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true);
						double xx2 = GST_nonHOP_query(mm, input_graph_Mock, select_groups2[i], true);
						double xx3 = GST_nonHOP_query(mm, input_graph_Mock, select_groups3[i], true);

						tuple<double, double, double> x = { xx1, xx2, xx3 };
						return x; // return to results; the return type must be the same with results
						})
				);
			}
			for (auto&& result : results) {
				auto xx = result.get(); //all threads finish here
				newHL_GST_time1.push_back(std::get<0>(xx)), newHL_GST_time2.push_back(std::get<1>(xx)), newHL_GST_time3.push_back(std::get<2>(xx));
			}
		}
		else {
			newHL_index_time = -1, newHL_index_size = -1;
			for (int i = 0; i < query_times; i++) {
				newHL_GST_time1.push_back(-1), newHL_GST_time2.push_back(-1), newHL_GST_time3.push_back(-1);
			}
		}
	}

	cout<< "step 4" <<endl;
	/*NOindex GST*/
	if (1) {
		ThreadPool pool(50);
		std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
		for (int i = 0; i < query_times; i++) {
			results.emplace_back(
				pool.enqueue([&input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i] { // pass const type value j to thread; [] can be empty

					auto b = std::chrono::high_resolution_clock::now();
					GST_nonHOP_NOINDEX(input_graph_Mock, select_groups1[i]);
					auto e = std::chrono::high_resolution_clock::now();
					double xx1 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

					b = std::chrono::high_resolution_clock::now();
					GST_nonHOP_NOINDEX(input_graph_Mock, select_groups2[i]);
					e = std::chrono::high_resolution_clock::now();
					double xx2 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

					b = std::chrono::high_resolution_clock::now();
					GST_nonHOP_NOINDEX(input_graph_Mock, select_groups3[i]);
					e = std::chrono::high_resolution_clock::now();
					double xx3 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

					tuple<double, double, double> x = { xx1, xx2, xx3 };
					return x; // return to results; the return type must be the same with results
					})
			);
		}
		for (auto&& result : results) {
			auto xx = result.get(); //all threads finish here
			NOINDEX_GST_time1.push_back(std::get<0>(xx)), NOINDEX_GST_time2.push_back(std::get<1>(xx)), NOINDEX_GST_time3.push_back(std::get<2>(xx));
		}
	}

   cout<< "step 5" <<endl;
	/* CT */
	if (1) {
		string algo_name = "CT";
		cout << "start " + algo_name << endl;

		CT_case_info mm;
		mm.thread_num = thread_num;
		mm.max_byte_size = max_byte_size;
		mm.max_run_time_seconds = max_run_time_seconds;
		mm.d = d;
		mm.use_P2H_pruning = 0;
		mm.two_hop_info.use_rank_prune = 0;
		mm.two_hop_info.use_2M_prune = 0;
		mm.two_hop_info.use_canonical_repair = 0;

		bool catch_error = false;
		try {
			CT(input_graph_Mock, mm);
		}
		catch (string s) {
			catch_error = true;
			cout << s << endl;
			clear_gloval_values_CT();
		}
		mm.record_all_details(save_name + "_d" + to_string(d) + "_" + algo_name);
		cout << "finish " + algo_name << endl;

		for (int i = 0; i < query_times; i++) {
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups1[i]) {
					avgMockL_size += mm.two_hop_info.L[x].size() + mm.Bags[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups1[i].size();
				CT_avgMockL_size1.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups2[i]) {
					avgMockL_size += mm.two_hop_info.L[x].size() + mm.Bags[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups2[i].size();
				CT_avgMockL_size2.push_back(avgMockL_size);
			}
			if (1) {
				double avgMockL_size = 0;
				for (auto x : select_groups3[i]) {
					avgMockL_size += mm.two_hop_info.L[x].size() + mm.Bags[x].size();
				}
				avgMockL_size = avgMockL_size / select_groups3[i].size();
				CT_avgMockL_size3.push_back(avgMockL_size);
			}
		}


		if (!catch_error) {
			CT_index_time = mm.time_total, CT_index_size = mm.compute_label_byte_size();
			ThreadPool pool(50);
			std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
			for (int i = 0; i < query_times; i++) {
				results.emplace_back(
					pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i] { // pass const type value j to thread; [] can be empty

						GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true); // this is to warm up this thread
						double xx1 = GST_nonHOP_query(mm, input_graph_Mock, select_groups1[i], true);
						double xx2 = GST_nonHOP_query(mm, input_graph_Mock, select_groups2[i], true);
						double xx3 = GST_nonHOP_query(mm, input_graph_Mock, select_groups3[i], true);

						tuple<double, double, double> x = { xx1, xx2, xx3 };
						return x; // return to results; the return type must be the same with results
						})
				);
			}
			for (auto&& result : results) {
				auto xx = result.get(); //all threads finish here
				CT_GST_time1.push_back(std::get<0>(xx)), CT_GST_time2.push_back(std::get<1>(xx)), CT_GST_time3.push_back(std::get<2>(xx));
			}
		}
		else {
			CT_index_time = -1, CT_index_size = -1;
			for (int i = 0; i < query_times; i++) {
				CT_GST_time1.push_back(-1), CT_GST_time2.push_back(-1), CT_GST_time3.push_back(-1);
			}
		}
	}



    cout<< "step 6" <<endl;
	/* WWW2020_newHL_Qratio */
	for (int i = 0; i < query_times; i++) {
		double min_ratio = min(WWW2020WithMock_GST_time1[i] / newHL_GST_time1[i], WWW2020WithMock_GST_time2[i] / newHL_GST_time2[i]);
		min_ratio = min(min_ratio, WWW2020WithMock_GST_time3[i] / newHL_GST_time3[i]);
		WWW2020_newHL_Qratio.push_back(min_ratio);
	}

	/*output*/
	for (int i = 0; i < query_times; i++) {
		outputFile << WWW2020NoMock_index_time << "," << WWW2020NoMock_index_size << "," <<
			WWW2020NoMock_GST_time1[i] << "," << WWW2020NoMock_GST_time2[i] << "," << WWW2020NoMock_GST_time3[i] << "," <<
			WWW2020WithMock_index_time << "," << WWW2020WithMock_index_size << "," << WWW2020WithMock_GST_time1[i] << "," << WWW2020Mock_avgMockL_size1[i] << "," <<
			WWW2020WithMock_GST_time2[i] << "," << WWW2020Mock_avgMockL_size2[i] << "," << WWW2020WithMock_GST_time3[i] << "," << WWW2020Mock_avgMockL_size3[i] << "," <<
			CT_index_time << "," << CT_index_size << "," << CT_GST_time1[i] << "," << CT_avgMockL_size1[i] << "," <<
			CT_GST_time2[i] << "," << CT_avgMockL_size2[i] << "," << CT_GST_time3[i] << "," << CT_avgMockL_size3[i] << "," <<
			newHL_index_time << "," << newHL_index_size << "," << newHL_GST_time1[i] << "," << newHL_avgMockL_size1[i] << "," <<
			newHL_GST_time2[i] << "," << newHL_avgMockL_size2[i] << "," << newHL_GST_time3[i] << "," << newHL_avgMockL_size3[i] << "," <<
			WWW2020_newHL_Qratio[i] << "," <<
			NOINDEX_GST_time1[i] << "," << NOINDEX_GST_time2[i] << "," << NOINDEX_GST_time3[i] << endl;
	}

	outputFile.close();
}

void exp_element_HOP_static(string data_name, int ec_type, int thread_num, long long int max_byte_size, double max_run_time_seconds) {

	/*read data*/
	string ec_type_name;
	if (ec_type == 0) {
		ec_type_name = "Jacard";
	}
	else if (ec_type == 1) {
		ec_type_name = "random";
	}
	graph_v_of_v<int> input_graph_Mock, input_graph_noMock;
	vector<int> new_is_mock;
	vector<vector<int>> select_groups1, select_groups2, select_groups3;
	input_graph_Mock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_Mock_" + ec_type_name + ".binary");
	input_graph_noMock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_noMock_" + ec_type_name + ".binary");
	binary_read_vector(global_path + data_name + "//exp_" + data_name + "_new_is_mock_" + ec_type_name + ".binary", new_is_mock);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups1_" + ec_type_name + ".binary", select_groups1);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups2_" + ec_type_name + ".binary", select_groups2);
	binary_read_vector_of_vectors(global_path + data_name + "//exp_" + data_name + "_select_groups3_" + ec_type_name + ".binary", select_groups3);
	is_mock = new_is_mock; // update is_mock
	int query_times = select_groups3.size();

	/*parameters*/
	vector<int> Ks = { 3, 5 };

	for (auto K : Ks) {

		/*output*/
		ofstream outputFile;
		outputFile.precision(8);
		outputFile.setf(ios::fixed);
		outputFile.setf(ios::showpoint);
		string save_name = "expHL4GST_HOP_" + data_name + "_" + ec_type_name + "_K" + to_string(K) + ".csv";
		cout << "start indexing " << save_name << endl;
		outputFile.open(save_name);

		outputFile << "WWW2023_index_time,WWW2023_index_size(byte),WWW2023_GST_time1,WWW2023_GST_time2,WWW2023_GST_time3,"
			<< "WWW2023MOCK_index_time,WWW2023MOCK_index_size(byte),WWW2023MOCK_GST_time1,WWW2023MOCK_avgMockL_size1,WWW2023MOCK_GST_time2,WWW2023MOCK_avgMockL_size2,WWW2023MOCK_GST_time3,WWW2023MOCK_avgMockL_size3,"
			<< "newHL_index_time,newHL_index_size(byte),newHL_GST_time1,newHL_avgMockL_size1,newHL_GST_time2,newHL_avgMockL_size2,newHL_GST_time3,newHL_avgMockL_size3,"
			<< "WWW2023_newHL_Qratio, NOINDEX_GST_time1, NOINDEX_GST_time2, NOINDEX_GST_time3" << endl;

		double WWW2023_index_time, WWW2023_index_size, WWW2023MOCK_index_time, WWW2023MOCK_index_size, CT_index_time, CT_index_size, newHL_index_time, newHL_index_size;

		vector<double> WWW2023_GST_time1, WWW2023_GST_time2, WWW2023_GST_time3,
			WWW2023MOCK_GST_time1, WWW2023MOCK_GST_time2, WWW2023MOCK_GST_time3, WWW2023MOCK_avgMockL_size1, WWW2023MOCK_avgMockL_size2, WWW2023MOCK_avgMockL_size3,
			newHL_GST_time1, newHL_GST_time2, newHL_GST_time3, WWW2023_newHL_Qratio, newHL_avgMockL_size1, newHL_avgMockL_size2, newHL_avgMockL_size3,
			NOINDEX_GST_time1, NOINDEX_GST_time2, NOINDEX_GST_time3;

		/* WWW2023_NOmock */
		if (1) {
			string algo_name = "WWW2023_NOmock";
			cout << "start " + algo_name << endl;

			/* hop bounded info */
			hop_constrained_case_info mm;
			mm.thread_num = thread_num;
			mm.max_run_time_seconds = max_run_time_seconds;
			mm.upper_k = K;
			mm.use_2M_prune = 0;
			mm.use_rank_prune = 0;
			mm.use_2023WWW_generation = 0;
			mm.use_canonical_repair = 0;

			bool catch_error = false;
			//try {
			//	hop_constrained_two_hop_labels_generation(input_graph_noMock, mm); // no mock
			//}
			//catch (string s) {
			//	catch_error = true;
			//	cout << s << endl;
			//	hop_constrained_clear_global_values();
			//}
			//mm.record_all_details(save_name + "_" + algo_name);
			cout << "finish " + algo_name << endl;

			catch_error = 1;

			if (!catch_error) {
				WWW2023_index_time = mm.time_total, WWW2023_index_size = mm.compute_label_byte_size();
				for (int i = 0; i < query_times; i++) {
					double xx1, xx2, xx3;
					try {
						GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, false); // this is to warm up this thread
						xx1 = GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, false);
					}
					catch (string s) {
						xx1 = INT_MAX;
					}
					try {
						xx2 = GST_HOP_query(mm, input_graph_Mock, select_groups2[i], 2 * K, false);
					}
					catch (string s) {
						xx2 = INT_MAX;
					}
					try {
						xx3 = GST_HOP_query(mm, input_graph_Mock, select_groups3[i], 2 * K, false);
					}
					catch (string s) {
						xx3 = INT_MAX;
					}
					WWW2023_GST_time1.push_back(xx1), WWW2023_GST_time2.push_back(xx2), WWW2023_GST_time3.push_back(xx3);
				}
			}
			else {
				WWW2023_index_time = -1, WWW2023_index_size = -1;
				for (int i = 0; i < query_times; i++) {
					WWW2023_GST_time1.push_back(-1), WWW2023_GST_time2.push_back(-1), WWW2023_GST_time3.push_back(-1);
				}
			}
		}

		/* WWW2023_mock */
		if (1) {
			string algo_name = "WWW2023_mock";
			cout << "start " + algo_name << endl;

			/* hop bounded info */
			hop_constrained_case_info mm;
			mm.thread_num = thread_num;
			mm.max_run_time_seconds = max_run_time_seconds;
			mm.upper_k = K;
			mm.use_2M_prune = 0;
			mm.use_rank_prune = 0;
			mm.use_2023WWW_generation = 0;
			mm.use_canonical_repair = 0;

			bool catch_error = false;
			try {
				hop_constrained_two_hop_labels_generation(input_graph_Mock, mm); // with mock
			}
			catch (string s) {
				catch_error = true;
				cout << s << endl;
				hop_constrained_clear_global_values();
			}
			mm.record_all_details(save_name + "_" + algo_name);
			cout << "finish " + algo_name << endl;

			for (int i = 0; i < query_times; i++) {
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups1[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups1[i].size();
					WWW2023MOCK_avgMockL_size1.push_back(avgMockL_size);
				}
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups2[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups2[i].size();
					WWW2023MOCK_avgMockL_size2.push_back(avgMockL_size);
				}
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups3[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups3[i].size();
					WWW2023MOCK_avgMockL_size3.push_back(avgMockL_size);
				}
			}



			if (!catch_error) {
				WWW2023MOCK_index_time = mm.time_total, WWW2023MOCK_index_size = mm.compute_label_byte_size();

				ThreadPool pool(50);
				std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
				for (int i = 0; i < query_times; i++) {
					results.emplace_back(
						pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i, K] { // pass const type value j to thread; [] can be empty

							GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, true); // this is to warm up this thread
							double xx1 = GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, true);
							double xx2 = GST_HOP_query(mm, input_graph_Mock, select_groups2[i], 2 * K, true);
							double xx3 = GST_HOP_query(mm, input_graph_Mock, select_groups3[i], 2 * K, true);

							tuple<double, double, double> x = { xx1, xx2, xx3 };
							return x; // return to results; the return type must be the same with results
							})
					);
				}
				for (auto&& result : results) {
					auto xx = result.get(); //all threads finish here
					WWW2023MOCK_GST_time1.push_back(std::get<0>(xx)), WWW2023MOCK_GST_time2.push_back(std::get<1>(xx)), WWW2023MOCK_GST_time3.push_back(std::get<2>(xx));
				}
			}
			else {
				WWW2023MOCK_index_time = -1, WWW2023MOCK_index_size = -1;
				for (int i = 0; i < query_times; i++) {
					WWW2023MOCK_GST_time1.push_back(-1), WWW2023MOCK_GST_time2.push_back(-1), WWW2023MOCK_GST_time3.push_back(-1);
				}
			}
		}
		else {
			WWW2023MOCK_index_time = -1, WWW2023MOCK_index_size = -1;
			for (int i = 0; i < query_times; i++) {
				WWW2023MOCK_GST_time1.push_back(-1), WWW2023MOCK_GST_time2.push_back(-1), WWW2023MOCK_GST_time3.push_back(-1);
			}
		}

		/* newHL */
		if (1) {
			string algo_name = "newHL";
			cout << "start " + algo_name << endl;

			/* hop bounded info */
			hop_constrained_case_info mm;
			mm.thread_num = thread_num;
			mm.max_run_time_seconds = max_run_time_seconds;
			mm.upper_k = K;
			mm.use_2M_prune = 1;
			mm.use_rank_prune = 1;
			mm.use_2023WWW_generation = 0;
			mm.use_canonical_repair = 1;

			if (data_name == "Pokec") {
				mm.use_canonical_repair = 0;
			}

			bool catch_error = false;
			try {
				hop_constrained_two_hop_labels_generation(input_graph_Mock, mm); // with mock
			}
			catch (string s) {
				catch_error = true;
				cout << s << endl;
				hop_constrained_clear_global_values();
			}
			mm.record_all_details(save_name + "_" + algo_name);
			cout << "finish " + algo_name << endl;

			for (int i = 0; i < query_times; i++) {
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups1[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups1[i].size();
					newHL_avgMockL_size1.push_back(avgMockL_size);
				}
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups2[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups2[i].size();
					newHL_avgMockL_size2.push_back(avgMockL_size);
				}
				if (1) {
					double avgMockL_size = 0;
					for (auto x : select_groups3[i]) {
						avgMockL_size += mm.L[x].size();
					}
					avgMockL_size = avgMockL_size / select_groups3[i].size();
					newHL_avgMockL_size3.push_back(avgMockL_size);
				}
			}

			if (!catch_error) {
				newHL_index_time = mm.time_total, newHL_index_size = mm.compute_label_byte_size();

				ThreadPool pool(50);
				std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
				for (int i = 0; i < query_times; i++) {
					results.emplace_back(
						pool.enqueue([&mm, &input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i, K] { // pass const type value j to thread; [] can be empty

							GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, true); // this is to warm up this thread							
							double xx1 = GST_HOP_query(mm, input_graph_Mock, select_groups1[i], 2 * K, true);
							double xx2 = GST_HOP_query(mm, input_graph_Mock, select_groups2[i], 2 * K, true);
							double xx3 = GST_HOP_query(mm, input_graph_Mock, select_groups3[i], 2 * K, true);

							tuple<double, double, double> x = { xx1, xx2, xx3 };
							return x; // return to results; the return type must be the same with results
							})
					);
				}
				for (auto&& result : results) {
					auto xx = result.get(); //all threads finish here
					newHL_GST_time1.push_back(std::get<0>(xx)), newHL_GST_time2.push_back(std::get<1>(xx)), newHL_GST_time3.push_back(std::get<2>(xx));
				}
			}
			else {
				newHL_index_time = -1, newHL_index_size = -1;
				for (int i = 0; i < query_times; i++) {
					newHL_GST_time1.push_back(-1), newHL_GST_time2.push_back(-1), newHL_GST_time3.push_back(-1);
				}
			}
		}

		/*NOindex GST*/
		if (1) {
			ThreadPool pool(50);
			std::vector< std::future<tuple<double, double, double>> > results; // return typename: xxx
			for (int i = 0; i < query_times; i++) {
				results.emplace_back(
					pool.enqueue([&input_graph_Mock, &select_groups1, &select_groups2, &select_groups3, i, K] { // pass const type value j to thread; [] can be empty

						auto b = std::chrono::high_resolution_clock::now();
						GST_HOP_NOINDEX(input_graph_Mock, select_groups1[i], 2 * K);
						auto e = std::chrono::high_resolution_clock::now();
						double xx1 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

						b = std::chrono::high_resolution_clock::now();
						GST_HOP_NOINDEX(input_graph_Mock, select_groups2[i], 2 * K);
						e = std::chrono::high_resolution_clock::now();
						double xx2 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

						b = std::chrono::high_resolution_clock::now();
						GST_HOP_NOINDEX(input_graph_Mock, select_groups3[i], 2 * K);
						e = std::chrono::high_resolution_clock::now();
						double xx3 = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count() / 1e9; // s

						tuple<double, double, double> x = { xx1, xx2, xx3 };
						return x; // return to results; the return type must be the same with results
						})
				);
			}
			for (auto&& result : results) {
				auto xx = result.get(); //all threads finish here
				NOINDEX_GST_time1.push_back(std::get<0>(xx)), NOINDEX_GST_time2.push_back(std::get<1>(xx)), NOINDEX_GST_time3.push_back(std::get<2>(xx));
			}
		}

		/* WWW2023_newHL_Qratio */
		for (int i = 0; i < query_times; i++) {
			double min_ratio = min(WWW2023MOCK_GST_time1[i] / newHL_GST_time1[i], WWW2023MOCK_GST_time2[i] / newHL_GST_time2[i]);
			min_ratio = min(min_ratio, WWW2023MOCK_GST_time3[i] / newHL_GST_time3[i]);
			WWW2023_newHL_Qratio.push_back(min_ratio);
		}

		/*output*/
		for (int i = 0; i < query_times; i++) {
			outputFile << WWW2023_index_time << "," << WWW2023_index_size << "," << WWW2023_GST_time1[i] << "," << WWW2023_GST_time2[i] << "," << WWW2023_GST_time3[i] << "," <<
				WWW2023MOCK_index_time << "," << WWW2023MOCK_index_size << "," << WWW2023MOCK_GST_time1[i] << "," << WWW2023MOCK_avgMockL_size1[i] << "," <<
				WWW2023MOCK_GST_time2[i] << "," << WWW2023MOCK_avgMockL_size2[i] << "," << WWW2023MOCK_GST_time3[i] << "," << WWW2023MOCK_avgMockL_size3[i] << "," <<
				newHL_index_time << "," << newHL_index_size << "," << newHL_GST_time1[i] << "," << newHL_avgMockL_size1[i] << "," <<
				newHL_GST_time2[i] << "," << newHL_avgMockL_size2[i] << "," << newHL_GST_time3[i] << "," << newHL_avgMockL_size3[i] << "," <<
				WWW2023_newHL_Qratio[i] << "," <<
				NOINDEX_GST_time1[i] << "," << NOINDEX_GST_time2[i] << "," << NOINDEX_GST_time3[i] << endl;
		}
		outputFile.close();
	}
}


void print_group_sizes_element(string data_name, int ec_type) {

	/*read data*/
	string ec_type_name;
	if (ec_type == 0) {
		ec_type_name = "Jacard";
	}
	else if (ec_type == 1) {
		ec_type_name = "random";
	}
	graph_v_of_v<int> input_graph_Mock;
	vector<int> new_is_mock;
	input_graph_Mock.binary_read(global_path + data_name + "//exp_" + data_name + "_input_graph_Mock_" + ec_type_name + ".binary");
	binary_read_vector(global_path + data_name + "//exp_" + data_name + "_new_is_mock_" + ec_type_name + ".binary", new_is_mock);

	int V = input_graph_Mock.size();

	/*output*/
	ofstream outputFile;
	outputFile.precision(8);
	outputFile.setf(ios::fixed);
	outputFile.setf(ios::showpoint);
	string save_name = "group_sizes_" + data_name + ".txt";
	outputFile.open(save_name);

	for(int i=0;i<V;i++){
		if(new_is_mock[i]){
			outputFile << input_graph_Mock[i].size() << endl;
		}
	}

	outputFile.close();
}


void print_group_sizes() {

	vector<string> used_datas = { "musae",  "twitch", "github", "amazon", "reddit", "dblp" };

	/*Jacard & random*/
	if (1) {
		for (int i = 0; i < used_datas.size(); i++) {
			print_group_sizes_element(used_datas[i], 0);
		}
	}

}

void main_exp_nonHOP() {

	//vector<string> used_datas = { "musae",  "twitch", "github", "amazon", "reddit", "dblp" };
	vector<string> used_datas = { "liveJournal" };

	long long int max_byte_size = pow(1024, 3) * 1000;
	double max_run_time_seconds = 3600 * 240;

	/*Jacard & random*/
	if (1) {
		for (int i = 0; i < used_datas.size(); i++) {
			exp_element_nonHOP_static(used_datas[i], 0, 80, max_byte_size, max_run_time_seconds);
		}
	}

}

void main_exp_HOP() {

	vector<string> used_datas = { "musae",  "twitch", "github", "amazon", "reddit", "dblp" };

	long long int max_byte_size = pow(1024, 3) * 500;
	double max_run_time_seconds = 3600 * 24;

	/*Jacard & random*/
	if (1) {
		for (int i = 0; i < used_datas.size(); i++) {
			exp_element_HOP_static(used_datas[i], 0, 80, max_byte_size, max_run_time_seconds);
		}
	}

}




int main()
{
	cout << "Start running..." << endl;
	auto begin = std::chrono::high_resolution_clock::now();
	srand(time(NULL)); //  seed random number generator

	//print_group_sizes();
	main_exp_nonHOP();
	//main_exp_HOP();

	auto end = std::chrono::high_resolution_clock::now();
	double runningtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1e9; // s
	cout << "END    runningtime: " << runningtime << "s" << endl;
}
