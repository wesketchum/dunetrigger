# Plane Coincidence Trigger

 - Serves as introductory low energy event trigger algorithm template. Based on the same method of continuous scanning of time windows containing TPs.
 - In the past we have used only collection channel TPC hits to trigger on, for example, horizontal muons in the VD ColdBox runs at CERN. With the introduction of induction TPs used in this algorithm, it is hoped they can be leveraged for the identification and triggering on candidate low energy events.
 - The current approach to developing this algorithm is the simulation of TPC TPs in LArSoft, using Klaudia's branch of [duneana](https://github.com/DUNE/duneana/tree/feature/kwawrows_TriggerTPGen/duneana/DAQSimAna). The output TPs can be played through the `trigger`
[replay app](https://github.com/DUNE-DAQ/trigger/tree/develop/python/trigger/replay_tp_to_chain), which can be used to test the functionality of algorithms offline, in `dunedaq` code.

## Trigger Activity Maker
- We have 3 instances of the `Window` class, as used in the Horizontal Muon Algorithm (HMA). Each `Window` will contain the TPs received in a configurable amount of time from one of three TPC planes. Currently we first require an ADC spike above pure radiological background and noise hits (from simulation) which is the sum of ADC integrals of all windows. Second, this must be coupled with a short primary ionisation track in the collection plane, presumably caused by a Michel electron or SN event. The adjacency checking of the tried and tested Horizontal Muon Algorithm is used here.

#### Adjacency Checking Function
 - This function returns the adjacency value for the current window, where adjacency
 is defined as the maximum number of consecutive wires containing hits. It accepts
 a configurable tolerance paramter, which allows up to adj_tolerance missing hits
 on adjacent wires before restarting the adjacency count. The maximum gap is 4 which
 comes from tuning on December 2021 coldbox data, and June 2022 coldbox runs.

 - Adjcancency Tolerance = Number of times prepared to skip missed hits before resetting 
  the adjacency count. This accounts for things like dead channels / missed TPs. The 
  maximum gap is 4 which comes from tuning on December 2021 coldbox data, and June 2022 
  coldbox runs.

## Trigger Candidate Maker
- At the moment, we use a trivial logic, by which we mean that there is a one-to-one map between emitted TAs and TCs. More complex triggering logic will be used in the future, when we want to cluster TAs from multiple detector elements (APAs).

 - For a talk on the related HMA, please see https://indico.fnal.gov/event/51502/contributions/226506/attachments/148509/190809/211019_FDDS_HorizontalMuon.pdf

