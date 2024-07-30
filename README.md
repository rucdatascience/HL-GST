# HL-GST

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



