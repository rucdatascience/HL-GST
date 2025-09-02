# L4G: Two-hop Label Management for Group Steiner Tree Search on Graphs

The introduction of these files are as follows. 



## HL-GST_data

The data files are zipped together, and should be unzipped first. There are 7 datasets: Musae, Twitch, Github, Amazon, DBLP, Reddit, LiveJournal. There are 7 files for each dataset. For example, the Musae dataset contains the following 7 files. 

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

- "exp_HL4GST.cpp" at "HL-GST_code\src\label_generation" contains codes for conducting the label generation and GST query experiments in the paper. 

- "exp_HL4GST_nonHOP_maintain.cpp" at "HL-GST_code\src\nonHOP_maintain" contains codes for conducting the label maintenance experiments in the paper. 

More detailed codes in other regions can be traced by the above codes. Specifically,

- "HL-GST_code\include\label_generation" contains source codes of implemented label generation algorithms in experiments.

- "HL-GST_code\include\nonHOP_maintain" contains source codes of implemented label maintenance algorithms in experiments.


## Compiling and Running Codes

To ensure a high reproducibility, we provide a unified workflow based on Docker. The following guide covers all steps from environment setup and code compilation for running experiments in the paper.

### Step 1: Prepare Project Files

First, the required project files must be prepared on the host machine. Ensure **Docker** is installed on your system.

1.  **Create the project directory structure.** Suppose the top-level project directory is `HL-GST`.
2.  **Download and unzip the datasets.** The unzipped data files should be placed in the `HL-GST/HL-GST_data/` directory.
3.  **Download and unzip the code.** The unzipped code files, including the `Dockerfile`, should be placed in the `HL-GST/HL-GST_code/` directory.

The final directory structure should be as follows:

```
HL-GST/
├── HL-GST_data/
│   ├── musae/
│   └── ...
└── HL-GST_code/
    ├── Dockerfile
    ├── CMakeLists.txt
    ├── run.sh
    ├── include/
    └── src/
```

### Step 2: Build Docker Image

We use the provided `Dockerfile` to build a self-contained image with all necessary dependencies, e.g., C++17 compiler, CMake, Boost library, etc. This process only needs to be performed once.

1.  **Navigate to the code directory.** Open a terminal and change the directory to `HL-GST/HL-GST_code/`, where the `Dockerfile` is located.

    ```bash
    cd /path/to/your/HL-GST/HL-GST_code
    ```


2.  **Build the Docker image.** Execute the following command to start the building process.

    ```bash
    sudo docker build -t hlgst-experiment .
    ```

    The `-t hlgst-experiment` argument assigns a convenient tag to the image. The `.` specifies that the `Dockerfile` is in the current directory.

### Step 3: Compile Codes using CMake

Once the image is built, a container can be launched to compile C++ source codes using CMake.

1.  **Create a results directory on the host.** This directory will be used to store output files from experiments.

    ```bash
    cd /path/to/your/HL-GST
    mkdir results
    ```

2.  **Launch the container.** The following command starts the container and places you inside its command-line shell.

    ```bash
    sudo docker run -it --rm \
      -v /path/to/your/HL-GST/HL-GST_data:/app/HL-GST_data \
      -v /path/to/your/HL-GST/results:/app/HL-GST_code/results \
      hlgst-experiment
    ```

    The `-v` flags mount host directories into the container, ensuring data persistence and access:

      * The first `-v` makes the host's `HL-GST_data` directory available inside the container at `/app/HL-GST_data`.
      * The second `-v` maps the host's `results` directory to `/app/HL-GST_code/results`, so that any generated result files are saved directly to the host.

3.  **Compile codes using CMake.** Inside the container (at the `/app/HL-GST_code` prompt), the codes are compiled using an out-of-source build.

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

    After a successful compilation, all executables will be located in the `/app/HL-GST_code/build/` directory.

### Step 4: Run Experiments

After compilation, experiments can be executed using the `run.sh` script.

**Note:** The script must be executed from the `/app/HL-GST_code` directory inside the container. If you are currently in the `build` directory, return to the parent directory with `cd ..`.

The following commands are supported by the `run.sh` script.

#### Main Experiment Commands

  * `sh run.sh gen-exp`: Runs experiments for generating 2-hop labels. It calls the `label_generator` executable.

  * `sh run.sh nonhop-lppr`: A pre-processing step for the maintenance experiments. It generates initial labels and indices required for maintenance tasks.

  * `sh run.sh nonhop-batch`: Runs the full batch of label maintenance experiments. This command must be executed after `sh run.sh nonhop-lppr` has completed successfully.


#### Test Commands

These commands execute unit tests to verify the correctness of different modules.

  * `sh run.sh gen-test`: Runs tests for the label generation module.
  * `sh run.sh nonhop-test`: Runs tests for the maintenance module.

All experiment results will be saved in the `results` directory on the host machine. To leave the container, simply type `exit`.

All the experiments in the paper are conducted via the above approaches.