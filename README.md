# Generation and Maintenance of 2-hop Labels for Accelerating Group Steiner Tree Search on Graphs

The introduction of these files are as follows. 



## HL-GST_data

The data files are zipped together, and should be unzipped first. There are six datasets: Musae, Twitch, Github, Amazon, DBLP, Reddit. There are 7 files for each dataset. For example, the Musae dataset contains the following 7 files. 

1. "exp_musae_info.txt". This readable file contains the basic information of this dataset, including the numbers of mock and non-mock vertices and edges in this dataset. This file also shows every edge and edge weight. For example, "Edge 0 1 1000000" shows that there is a mock edge between vertex 0 and vertex 1, with the constant mock edge weight of 1000000; and "Edge 0 4 51" shows that there is a non-mock edge between vertex 0 and vertex 4, with a non-mock edge weight of 51, which corresponds to a pairwise Jaccard distance of 0.51. Notably, mock vertices are only adjacent to non-mock vertices through mock edges, and each mock edge connects a mock vertex and a non-mock vertex. Thus, the above two edges indicate that vertex 0 and vertex 4 are non-mock vertices, while vertex 1 is a mock vertex.

The following 6 binary files can be read by codes when conducting experiments. The reason for generating binary files is that it is much faster to read binary files than to read raw data files when conducting experiments.

2. "exp_musae_new_is_mock_Jacard.binary". This binary file uses bool values to show the mock or non-mock state of each vertex.

3. "exp_musae_input_graph_Mock_Jacard.binary". This binary file contains an adjacency list of the tranformed graph with both mock and non-mock components.

4. "exp_musae_input_graph_noMock_Jacard.binary". This binary file contains an adjacency list of the original graph with only non-mock components.

5. "exp_musae_select_groups1_Jacard.binary". This binary file contains the randomly selected sets of vertex groups for querying group Steiner trees in experiments. Each set contains 3 vertex groups. 

6. "exp_musae_select_groups2_Jacard.binary". This binary file contains the randomly selected sets of vertex groups for querying group Steiner trees in experiments. Each set contains 5 vertex groups. 

7. "exp_musae_select_groups3_Jacard.binary". This binary file contains the randomly selected sets of vertex groups for querying group Steiner trees in experiments. Each set contains 7 vertex groups. 


## HL-GST_code

There are some h and cpp files for conducting experiments in the paper. The h files are at "HL-GST_code\include", while the cpp files are at "HL-GST_code\src". In particular,

- "exp_HL4GST.cpp" at "HL-GST_code\src\label_generation" contains codes for conducting the label generation experiments in the paper. 

- "exp_HL4GST_nonHOP_maintain.cpp" at "HL-GST_code\src\nonHOP_maintain" contains codes for conducting the unconstrained label maintenance experiments in the paper. 

- "exp_HL4GST_HOP_maintain.cpp" at "HL-GST_code\src\HOP_maintain" contains codes for conducting the constrained label maintenance experiments in the paper. 

More detailed codes in other regions can be traced by the above codes for experiment. Specifically,

- "HL-GST_code\include\label_generation" contains source codes of the implemented algortihms in the label generation experiments.

- "HL-GST_code\include\nonHOP_maintain" contains source codes of the implemented algortihms in the unconstrained label maintenance experiments.

- "HL-GST_code\include\HOP_maintain" contains source codes of the implemented algortihms in the constrained label maintenance experiments.





## Compiling Code example

We compile the above codes and run the experiments in the paper on a Linux server with the Ubuntu 20.04.1 system, two Intel Xeon Gold 6342 processors, 1 TB RAM and 2 TB hard disk space. We show the details as follows.

Suppose that the user name on the Linux server is "root". We prepare the compiling environment as follows.

- <b>Download and unzip the datasets.</b> Suppose that the unzipped datasets are at "root/HL-GST_data" on the Linux server.


- <b>Download and unzip the Boost library at https://www.boost.org.</b> Suppose that the unzipped Boost files are at "home/boost_1_75_0" on the Linux server.


- <b>Download and unzip the codes.</b> Suppose that the unzipped codes are at "root/HL-GST_code" on the Linux server.

After preparing the environment as suggested above, we can compile and run the codes using the sh files at "HL-GST_code\sh". 

Specifically, in the root terminal on the Linux server, we can use the following command to run the experiments of generating 2-hop labels:
```
sh HL-GST_code/sh/run_label_generation_exp.sh
```

The contents in "run_label_generation_exp.sh" are as follows:
```
g++ -std=c++17 -I/home/boost_1_75_0 -I/root/HL-GST_code/include /root/HL-GST_code/src/label_generation/exp_HL4GST.cpp -lpthread -O3 -o A
./A
```
where "-I/home/boost_1_75_0" is to add the path of the boost folder when compiling, "-I/root/HL-GST_code/include" is to add the path of the cppheader folder when compiling, "/root/HL-GST_code/src/label_generation/exp_HL4GST.cpp" is the compiled cpp file for the experiments of generating 2-hop labels, "-lpthread" is for parallel computation, and "-O3" is for compiler optimisation.

The experiment results will be automatically saved in CSV files.




To run the experiments of maintaining unconstrained labels, we need to first use the following command to generate the initial unconstrained labels:
```
sh HL-GST_code/sh/run_nonHOP_maintain_exp_generateLPPR.sh
```
and then use the following command to run the experiments of maintaining unconstrained labels:
```
sh HL-GST_code/sh/run_nonHOP_maintain_exp.sh
```




Similarly, to run the experiments of maintaining constrained labels, we need to first use the following command to generate the initial constrained labels:
```
sh HL-GST_code/sh/run_HOP_maintain_exp_generateLPPR.sh
```
and then use the following command to run the experiments of maintaining constrained labels:
```
sh HL-GST_code/sh/run_HOP_maintain_exp.sh
```

All the experiments in the paper are conducted via the above approaches.

























