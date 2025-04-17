#ifndef DUNETRIGGER_TRIGGERSIM_TPALGTPCSIMPLETHRESHOLD_hh
#define DUNETRIGGER_TRIGGERSIM_TPALGTPCSIMPLETHRESHOLD_hh

#include "fhiclcpp/ParameterSet.h" 

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcore/Geometry/WireReadout.h"

#include "dunetrigger/TriggerSim/TPAlgTools/TPAlgTPCTool.hh"
#include "dunetrigger/TriggerSim/Verbosity.hh"

#include <map>
#include <limits>
#include <inttypes.h>

namespace duneana {

 class TPAlgTPCSimpleThreshold : public TPAlgTPCTool {

  public:
    explicit TPAlgTPCSimpleThreshold(fhicl::ParameterSet const& ps) :
      verbosity_(ps.get<int>("verbosity",0)),
      accum_limit_(ps.get<int>("accum_limit",10)),
      threshold_tpg_plane0_(ps.get<int16_t>("threshold_tpg_plane0")),
      threshold_tpg_plane1_(ps.get<int16_t>("threshold_tpg_plane1")),
      threshold_tpg_plane2_(ps.get<int16_t>("threshold_tpg_plane2"))
    {}

    void initialize_channel_state(dunedaq::trgdataformats::channel_t const& channel,
                                  std::vector<short> const& adcs)
    {

        //grab the geometry service
        geo::WireReadoutGeom const* geom = &art::ServiceHandle<geo::WireReadout>()->Get();
        auto plane = geom->ROPtoWirePlanes(geom->ChannelToROP(channel)).at(0).Plane;

        if(plane==0) threshold_=threshold_tpg_plane0_;
        else if(plane==1) threshold_=threshold_tpg_plane1_;
        else if(plane==2) threshold_=threshold_tpg_plane2_;

        if(verbosity_ >= Verbosity::kInfo) {
            std::cout << "Channel: " << channel
                      << ", ROP: " << plane
                      << ", Threshold: " << threshold_
                      << std::endl;
        }

        //find the mode of the whole vector
        std::map<short,size_t> counts_per_value;
        for(auto const& adc : adcs)
	  counts_per_value[adc] += 1;
        size_t max_counts=0;
        for(auto it=counts_per_value.begin(); it!= counts_per_value.end(); ++it) {
	  if (it->second > max_counts){
	    pedestal_ = it->first;
	    max_counts = it->second;
	  }
        }
	
        accum_ = 0;
	
        prev_was_over_=0;
        hit_charge_=0;
        hit_tover_=0;
        hit_peak_adc_=0;
        hit_peak_time_=0;
	
    }

    void frugal_accum_update(const int16_t sample)
    {
        if (sample > pedestal_) ++accum_;
        if (sample < pedestal_) --accum_;

        if (accum_ > accum_limit_) {
            ++pedestal_;
            accum_ = 0;
        }
        if (accum_ < -1 * accum_limit_) {
            --pedestal_;
            accum_ = 0;
        }
    }

    void process_waveform(std::vector<short> const& adcs,
			  dunedaq::trgdataformats::channel_t const channel,
			  dunedaq::trgdataformats::detid_t const detid,
			  dunedaq::trgdataformats::timestamp_t const start_time,
			  std::vector<dunedaq::trgdataformats::TriggerPrimitive> & tps_out) 
    {
        //setup a TP and initialize it with the common things for this algorithm/channel
        dunedaq::trgdataformats::TriggerPrimitive this_tp;
      
        this_tp.channel = channel;
        this_tp.detid = detid;
        this_tp.type = dunedaq::trgdataformats::TriggerPrimitive::Type::kTPC;
        this_tp.algorithm = dunedaq::trgdataformats::TriggerPrimitive::Algorithm::kSimpleThreshold;
        this_tp.flag = 0;

        //for this channel, reinitialize the channel state variables
        initialize_channel_state(channel, adcs);

        for(size_t i_t=0; i_t<adcs.size(); ++i_t){

	  //if threshold < 0, the plane is not used to produce TPs
	  if (threshold_ < 0) continue; 

            //get the sample
	    int16_t sample = adcs[i_t];

            //update the pedestal estimate
            frugal_accum_update(sample);

            //pedestal subtract
            sample -= pedestal_;

            //check if we are over threshold
            bool is_over = sample > threshold_;
            if(is_over)
            {
                //we are over threshold, so need to update the hit charge and check for peak time

                //first, need to saturate hit_charge at int16
                int32_t tmp_charge = hit_charge_;
                tmp_charge += sample;
                // tmp_charge = std::min(tmp_charge, (int32_t)std::numeric_limits<int16_t>::max()); // 32767

                //check if we're at the peak adc
                if(sample > hit_peak_adc_){
                    hit_peak_adc_ = (uint16_t)sample;
                    hit_peak_time_ = hit_tover_;
                }

                //update charge and time over threshold
                hit_charge_ = (uint16_t)tmp_charge;
                ++hit_tover_;
            }
            if(prev_was_over_ && !is_over)
            {
                //we've reached the end of the hit, so need to create a TP and write it out

	        this_tp.time_start = start_time + (i_t - hit_tover_ )*this->ADC_SAMPLING_RATE_IN_DTS;
                this_tp.time_over_threshold = hit_tover_*this->ADC_SAMPLING_RATE_IN_DTS;
                this_tp.time_peak = start_time + (i_t - hit_tover_ + hit_peak_time_)*this->ADC_SAMPLING_RATE_IN_DTS;
                this_tp.adc_integral = hit_charge_;
                this_tp.adc_peak = hit_peak_adc_;
                tps_out.push_back(this_tp);

                //now reset the hit variables
                hit_charge_=0;
                hit_tover_=0;
                hit_peak_adc_=0;
                hit_peak_time_=0;
            }
            prev_was_over_ = is_over;

        }



    }
    
  private:

    //configuration parameters
    const int verbosity_;
    const int accum_limit_;
    const int16_t threshold_tpg_plane0_;
    const int16_t threshold_tpg_plane1_;
    const int16_t threshold_tpg_plane2_;

    //variables for tracking state / hit-finding
    int16_t threshold_;
    int16_t pedestal_;
    int16_t accum_;
    int16_t accum25_;
    int16_t accum75_;

    uint16_t prev_was_over_;
    uint16_t hit_charge_;
    uint16_t hit_tover_;
    uint16_t hit_peak_time_;
    uint16_t hit_peak_adc_;

  };
  
}

#endif
