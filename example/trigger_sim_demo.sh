#!/bin/bash

output_data_dir=$1
n_events=10

#cosmics generation
lar -c prod_cosmics_1GeV_protodunehd.fcl -n $n_events -o $output_data_dir/test_cosmics.root

#g4 stage
lar -c standard_g4_protodunehd.fcl -n -1 -s $output_data_dir/test_cosmics.root -o $1/test_g4.root

#detsim stage
lar -c standard_detsim_protodunehd.fcl -n -1 -s $output_data_dir/test_g4.root -o $1/test_detsim.root

# trigger
lar -c run_triggersim_onlinealgs.fcl -n -1 -s $output_data_dir/test_detsim.root -o $output_data_dir/trigger.root

