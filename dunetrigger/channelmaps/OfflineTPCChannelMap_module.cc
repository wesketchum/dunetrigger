#include "dunetrigger/channelmaps/OfflineTPCChannelMap.hpp"
#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>
#include "larcore/Geometry/Geometry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include <memory>

// before anyone gets mad this really makes a lot of sense here
// and, crucially, is in the implementation and not the header
// so it does not propagate
using namespace dunedaq::detchannelmaps;

uint TPCChannelMap::get_plane_from_offline_channel(uint offchannel){
    art::ServiceHandle<geo::Geometry> geom;
    readout::ROPID rop = geom->ChannelToROP(static_cast<raw::ChannelID_t>(offchannel));
    return rop.deepestIndex();
}

std::shared_ptr<TPCChannelMap> dunedaq::detchannelmaps::make_map(std::string const& plugin_name){
    return std::shared_ptr<TPCChannelMap>();
};