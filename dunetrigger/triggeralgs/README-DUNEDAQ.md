# triggeralgs
A place to put the trigger algorithms for DUNE. This code can then be
linked inside ArtDAQ and LArSoft, so that we can run the exact same
code in simulation and on real data.

### TOC
 - [Future](#future)
 - [Organisation](#organisation)
 - [Compile](#compile)
 - [Contribute](#contribute)
 - [Data structures](#structs)

<a name="future">

## Future interesting things to do
 - Improve documentation!!
 - Make the whole setup more realistic
   - TPs from PDFs of what we expect from LArSoft/simulations
   - TAs algorithm better
 - Find a way to estimate the efficiency for TAs and TCs.
 - Implementation of "TP window" and "TP zipper":
   - TP window = something that ensures that the TPs are all in a time window,
   - TP zipper = something that merges source of TPs.


<a name="organisation"/>

## Organisation
The code is quite simple. Essentially, the trigger algorithm rely on 3
structs:
 - `TriggerPrimitive` (also known as hits)
 - `TriggerActivity` (clusters)
 - `TriggerCandidate` (trigger)
 - `TriggerDecision` (readout request ?)
 
Their format will be discussed at a DAQ meeting and fixed at some point.

The trigger primitives are created within ArtDAQ or LArSoft. These are
passed to one or more `TriggerActivityMaker` classes which create the
`TriggerActivity`. The `TriggerActivity` are then passed on to one
or more `TriggerCandidateMaker` classes, which create the
candidates, which in turn will be fed to `TriggerDecisionMaker`, etc. Each rely on their pure virtual operator:
 - `void TriggerActivityMaker ::operator()(const TriggerPrimitive& input_tp, std::vector<TriggerActivity >& output_ta)`
 - `void TriggerCandidateMaker::operator()(const TriggerActivity& input_ta , std::vector<TriggerCandidate>& output_tc)`
 - `void TriggerDecisionMaker ::operator()(const TriggerCandidate& input_tc, std::vector<TriggerDecision >& output_td)`

that any `TriggerActivityMaker`, `TriggerCandidateMaker` and
`TriggerDecisionMaker` need to implement, respectively.

Note the TPs can also be created here, but given how this happens now
in real life, it doesn't look like these libraries will be used for
creating TPs (the data structures for the raw data are very
complicated and closely tied to ArtDAQ, that is not so much the case
in LArSoft), hence the `TriggerPrimitiveMaker` class exists with its
corresponding operator:
- `void TriggerPrimitiveMaker::operator()(const void* data, std::vector<TriggerPrimitive>& tps)`

but that may not necessarily be used.

These operators do all the work, they should be reasonably
fast to handle the rate at which their input arrive in the real
system. The "makers" get input data, rearrange it, and then
`push_back` to the output vector. An example is available in
`src/trivial/TriggerCandidateMaker_trivial.hh`. It can be tested with
a `exec/run_trivial_candidate.cxx`, which creates fake TPs, feeds
them into it and then dumps them in a csv.

Note none the granularity of the `TriggerActivityMaker`,
`TriggerCandidateMaker` and `TriggerDecisionMaker` isn't be decided here. 
It is the ArtDAQ's developers' job to decided how many
`TriggerCandidateMakers` there should be in one APA for example. The
focus of this library is the algorithm, ArtDAQ should
create and handle the DuneTriggers objects and the calls to them (this is realised in triggermodules)

<a name="compile"/>

## To compile it
You need to have the boost libraries for testing (this dependency will be dropped soon, we don't need it)
```
git clone https://github.com/DUNE-DAQ/triggeralgs.git
cd triggeralgs
mkdir build
cd build
export TRIGGERALGS_DIR=$(pwd)/../install # this is for triggermodules to know where this install is.
cmake -DCMAKE_INSTALL_PREFIX=${TRIGGERALGS_DIR} ../
make -j`nproc`
make install # to move all the insteresting stuff in ${TRIGGERALGS_DIR}
ctest # if you want to run the tests, right now it doesn't do anything interesting
```

<a name="contribute"/>

## To contribute
Fork this repo.

<a name="structs"/>

## More details on the structs
Note all the times here refer to 50 MHz clock (since epoch), as is now standard in DUNE.

### TriggerPrimitive
This struct represent hits, it contains:
| Variable              | Type       | Comment                                                                   |
|-----------------------|------------|---------------------------------------------------------------------------|
| `time_start`          | `int64_t`  | Start time                                                                |
| `time_peak`           | `int64_t`  | Time of the peak ADC                                                      |
| `time_over_threshold` | `int32_t`  | Time over threshold                                                       |
| `channel`             | `uint32_t` | Channel number                                                            |
| `adc_integral`        | `uint16_t` | ADC integral                                                              |
| `adc_peak`            | `uint16_t` | ADC peak (could be 12 bits, strictly)                                     |
| `detid`               | `uint32_t` | detid (a flag that represents the detector which created the TP)          |
| `type`                | `uint32_t` | some flag that says what it think it is (PDS/TPC for example)             |
| `algorithm`           | `uint32_t` | some flag that says which algorithm created it (CPU/Firmware for example) |
| `version`             | `uint16_t` | version of above                                                          |
| `flag`                | `uint32_t` | extra flags.                                                              |

### TriggerActivity
Represents a cluster of hits, it contains:
| Variable        | Type                            | Comment                                                                                   |
|-----------------|---------------------------------|-------------------------------------------------------------------------------------------|
| `time_start`    | `int64_t`                       | Start time                                                                                |
| `time_end`      | `int64_t`                       | End time                                                                                  |
| `time_peak`     | `int64_t`                       | Time where ADC the highest                                                                |
| `time_formed`   | `int64_t`                       | Time at which the object was created (maybe we don't need that?)                          |
| `channel_start` | `uint32_t`                      | Start channel                                                                             |
| `channel_end`   | `uint32_t`                      | End channel                                                                               |
| `channel_peak`  | `uint32_t`                      | Channel of the highest ADC                                                                |
| `adc_integral`  | `uint16_t`                      | ADC integral                                                                              |
| `adc_peak`      | `uint16_t`                      | ADC peak (could be 12 bits, strictly)                                                     |
| `detid`         | `uint32_t`                      | detid (a flag that represents the detector in which it ocurred)                           |
| `type`          | `uint32_t`                      | some flag that says what it think it is (Argon39/Muon for example)                        |
| `algorithm`     | `uint32_t`                      | some flag that says which algorithm created it (PDS/TPS/SN/high energy/solar for example) |
| `version`       | `uint16_t`                      | version of above                                                                          |
| `tp_list`       | `std::vector<TriggerPrimitive>` | the list of TPs that was used to create it                                                |
 
### TriggerCandidate
contains a trigger "request" (i.e. please trigger). In this case, it's based on activity in the detector, but it could be external trigger as well, if one removes the `ta_list`:
| Variable       | Type                           | Comment                                                                           |
|----------------|--------------------------------|-----------------------------------------------------------------------------------|
| `time_start`   | `int64_t`                      | Time at which to start the readout                                                |
| `time_end`     | `int64_t`                      | Time at which to end the readout                                                  |
| `time_decided` | `int64_t`                      | Time at which this decision was taken                                             |
| `detid`        | `uint32_t`                     | some flag that represents which part of the detector to readout                   |
| `type`         | `uint32_t`                     | some flag that says what it think it is (SN/Muon/Beam for example)                |
| `algorithm`    | `uint32_t`                     | some flag that says which algorithm created it (SN/high energy/solar for example) |
| `version`      | `uint16_t`                     | version of above                                                                  |
| `ta_list`      | `std::vector<TriggerActivity>` | the list of TAs that was used to create it                                        |

### TriggerDecision
contains a readout request (i.e. you MUST trigger), based on all the candidates in the detector:
| Variable         | Type                            | Comment                                                                                               |
|------------------|---------------------------------|-------------------------------------------------------------------------------------------------------|
| `time_start`     | `int64_t`                       | Time at which to start the readout                                                                    |
| `time_end`       | `int64_t`                       | Time at which to end the readout                                                                      |
| `time_triggered` | `int64_t`                       | Time at which this decision was taken                                                                 |
| `detid`          | `uint32_t`                      | some flag that represents which part of the detector to readout                                       |
| `type`           | `uint32_t`                      | some flag that says what it think it is (SN/Muon/Beam for example)                                    |
| `algorithm`      | `uint32_t`                      | some flag that says which algorithm created it (although I think there will only ever be one of this) |
| `version`        | `uint16_t`                      | version of above                                                                                      |
| `tc_list`        | `std::vector<TriggerCandidate>` | the list of TCs that was used to create it                                                            |
