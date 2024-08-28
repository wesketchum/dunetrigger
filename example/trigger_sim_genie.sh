#!/bin/bash

output_data_dir=$1
n_events=10


#event generation
lar -c prodgenie_nu_dune10kt_1x2x6.fcl -n $n_events -o $output_data_dir/genie_events.root

#g4 stage
lar -c standard_g4_dune10kt_1x2x6.fcl -n -1 -s $output_data_dir/genie_events.root -o $output_data_dir/genie_g4.root

#detsim stage
lar -c detsim_1dsimulation_dune10kt_1x2x6.fcl -n -1 -s $output_data_dir/genie_g4.root -o $output_data_dir/genie_detsim.root

# trigger
lar -c run_triggersim_fd.fcl -n -1 -s $output_data_dir/genie_detsim.root -o $output_data_dir/genie_trigger.root

