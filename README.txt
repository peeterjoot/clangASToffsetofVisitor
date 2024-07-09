This is a clang AST visitor that attempts to print out offsetof() info for any structures encountered.

It currently has trouble with anonymous structures (doesn't always find the name), and also inappropriately prints out offsetof() info for bitfield types
(well that's inappropriate if the use case is static_assert'ions using that offsetof info, since offsetof() itself cannot access bitfield types.)

=============================

To attempt to run with ASAN:

sudo yum -y install libasan

run it w/ asan:

LD_PRELOAD=/usr/lib64/libasan.so.5.0.0 ../visitor t2.cpp
