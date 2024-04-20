#!/bin/bash

output_data_dir=/exp/dune/data/users/wketchum/trigger_sim_testing
n_events=10

#cosmics generation
lar -c prod_cosmics_1GeV_protodunehd.fcl -n $n_events -o $output_data_dir/test_cosmics.root

#g4 stage
lar -c standard_g4_protodunehd.fcl -n -1 -s $output_data_dir/test_cosmics.root -o /exp/dune/data/users/wketchum/trigger_sim_testing/test_g4.root

#detsim stage
lar -c standard_detsim_protodunehd.fcl -n -1 -s $output_data_dir/test_g4.root -o /exp/dune/data/users/wketchum/trigger_sim_testing/test_detsim.root

