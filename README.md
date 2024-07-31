# HL-GST

The introduction of these files are as follows. 



## HL-GST_data

The data files are zipped together, and should be unzipped first. There are six datasets: Musae, Twitch, Github, Amazon, DBLP, Reddit. There are 7 files for each dataset. For example, the Musae dataset contains the following files: 

1. "exp_musae_info.txt". This readable file contains the basic information of this dataset, including the numbers of mock and nonmock vertices and edges in this dataset. This file also shows every edge and edge weight in this dataset. For example, "Edge 0 1 1000000" shows that there is a mock edge between vertex 0 and vertex 1, with the constant mock edge weight of 1000000; and "Edge 0 4 51" shows that there is a non-mock edge between vertex 0 and vertex 4, with a non-mock edge weight of 51, which corresponds to a pairwise Jaccard distance of 0.51. Notably, mock vertices are only adjacent to non-mock vertices through mock edges, and each mock edge connects a mock vertex and a non-mock vertex. Thus, the above two edges indicate that vertex 0 and vertex 4 are non-mock vertices, while vertex 1 is a mock vertex.

2. "exp_musae_new_is_mock_Jacard.binary".

3. "exp_musae_input_graph_Mock_Jacard.binary".

4. "exp_musae_input_graph_noMock_Jacard.binary".

5. "exp_musae_select_groups1_Jacard.binary".

6. "exp_musae_select_groups2_Jacard.binary".

6. "exp_musae_select_groups3_Jacard.binary".







 (zipped together; should be unzipped first). 

There are sie files for each dataset. For example, the Amazon dataset contains the following files: 
1. amazon_edges.txt: the edges, as well as random and Jacard edge weights.
2. amazon_vertex_names.txt: the vertices as well as vertex names.
3. amazon_graph_Jacard_ec.binary: the binary file of an adjacency list of the Amazon graph with Jacard weights. This file will be read by the following codes.
4. amazon_graph_random_ec.binary: the binary file of an adjacency list of the Amazon graph with random weights. This file will be read by the following codes.
5. amazon_MDC_query_list.binary: the binary file of the randomly generated query list for the indexing algorithms in the main experiments (each query is a pair of dummy and non-dummy vertices). This file will be read by the following codes.
6. amazon_query_list.binary: the binary file of the randomly generated query list for the GST and MDC algorithms in the main experiments (each query is a set of dummy vertices, respresenting the set of vertex groups in the GST and MDC problems). This file will be read by the following codes.









# Data

The data files are zipped together, and should be unzipped first. There are 6 datasets in total, and there are 3 files for each dataset. For example, the Amazon dataset contains the following files: 
1. amazon_graph.txt: the readable contents for the amazon graph, with Jaccard and random edge weights inside.
2. amazon_Jaccard.bin: the binary file of the amazon graph with Jaccard edge weights (the reason for generating binary files is that it is much faster to read binary files than to read raw data files).
3. amazon_random.bin: the binary file of the amazon graph with random edge weights.



# C++ codes 

The codes for conducting the experiments are in <b>exp.cpp</b>. 

The codes for the algorithms are in the header files in cppheaders_202*****.zip.

To read these C++ codes in detail, it is recommended to start from "exp()". More detailed codes in other regions can then be traced. In particular, in the cppheaders_202***** folder,
- "build_in_progress/HL/dynamic/PLL_dynamic.h" contains codes of <b>PLL</b> to generate L and PPR.
- "build_in_progress/HL/dynamic/WeightIncreaseMaintenance_improv.h" contains codes of the proposed weight increase maintenance algorithm (<b>FastInM</b>).
- "build_in_progress/HL/dynamic/WeightDecreaseMaintenance_improv.h" contains codes of the proposed weight decrease maintenance algorithm (<b>FastDeM</b>).
- "build_in_progress/HL/dynamic/WeightIncrease2021.h" contains codes of the existing <b>InAsyn</b> algorithm.
- "build_in_progress/HL/dynamic/WeightDecrease2021.h" contains codes of the repaired <b>RepairedDeAsyn</b> algorithm.
- "build_in_progress/HL/dynamic/WeightDecrease2014.h" contains codes of the existing <b>DePLL</b> algorithm.
- "build_in_progress/HL/dynamic/WeightIncrease2019.h" contains codes of the existing <b>InPLL</b> algorithm.



# Settings

To compile and run exp.cpp, prepare the environment as follows:

- download and unzip the header files in cppheaders_202*****.zip
- download the Boost library at https://www.boost.org
- download and unzip the datasets
- get a server with enough memory (1 TB RAM) and hard disk space (1 TB). (to only run the small CondMat and Gnutella datasets, just personal computers are OK)

After preparing the environment as suggested above, in the terminal on a Linux server, we can compile and run exp.cpp using the following commands:
```
g++ -std=c++17 -I/home/boost_1_75_0 -I/root/cppheaders_202***** exp.cpp -lpthread -O3 -o A.out
./A.out
```
, where "-I/home/boost_1_75_0" is to add the path of the boost folder when compiling, "-I/root/cppheaders_202*****" is to add the path of the cppheader folder when compiling, "-lpthread" is for parallel computation, and "-O3" is for compiler optimisation.

Specifically, <b>we can first run "generate_L_PPR()" in "exp.cpp" to generate all the initial shortest distances indexes, and then run "exp()" in "exp.cpp" to conduct all the experiments in the paper.</b> 

All the experiments in the paper are conducted via the above approach, on a Linux server with the Ubuntu 20.04.1 system, two Intel
Xeon Gold 6342 processors, 1 TB RAM and 2 TB hard disk space.




# Indexing Shortest Paths Between Vertices and Vertex Groups in Weighted Complex Networks

# C++ codes 

The C++ source codes are in <b>HL_dummy_202*****.cpp</b>. 

Running these codes requires including some header files (e.g. #include <graph_hash_of_mixed_weighted/graph_hash_of_mixed_weighted.h>; see cppheader_202*****.zip) and the Boost library (e.g. #include <boost/random.hpp>; https://www.boost.org/) . 

We use the above datasets to generate the above binary files for the experiments. The reason for generating binary files is that it is much faster to read binary files than to read raw data files. 

After making the header files and binary files ready, <b>all the experiments in our paper can be conducted by running the function "HL_main_exp()"</b>. Make sure there is enough memory (500 GB memory). 

Specifically, in the terminal on a Linux server, we can compile and run the above codes using the following commands:
```
g++ -std=c++17 -I/home/boost_1_75_0 -I/root/cppheader_202***** HL_dummy_202*****.cpp -lpthread -O3 -o A.out
./A.out
```
, where "-I/home/boost_1_75_0" is to add the path of the boost folder when compiling, "-I/root/cppheader_202*****" is to add the path of the cppheader folder when compiling, "-lpthread" is for parallel computation, and "-O3" is for compiler optimisation.

To read these C++ codes in detail, it is recommended to start from "HL_main_exp()", and then go to "exp_element()". More detailed codes in other regions can then be traced, such as the codes of proposed algorithms in "graph_hash_of_mixed_weighted/HL/two_hop_v2/graph_hash_of_mixed_weighted_CT_v2.h"
















Compile codes on a linux server:

Suppose that the data are at /root/HL-GST_data

Suppose that the code are at /root/HL-GST_code

Then, on a terminal at the root position, you can run the codes using sh files. E.g.,

sh HL-GST_code/sh/run_label_generation_test.sh

inside commands:

g++ -std=c++17 -I/home/boost_1_75_0 -I/root/HL-GST_code/include /root/HL-GST_code/src/label_generation/test_HL4GST.cpp -lpthread -O3 -o A
./A


-I/home/boost_1_75_0   is the boost file location

-I/root/HL-GST_code/include  is the headers location

/root/HL-GST_code/src/label_generation/test_HL4GST.cpp  to compile this cpp file



