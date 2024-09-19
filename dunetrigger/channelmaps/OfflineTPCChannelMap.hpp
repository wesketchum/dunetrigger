#ifndef DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP
#define DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP

#include <memory>
#include <optional>
#include <string>

namespace dunedaq::detchannelmaps {

class TPCChannelMap {
public:
  struct TPCCoords {
    uint16_t crate;
    uint16_t slot;
    uint16_t fiber;
    uint16_t channel;
  };

  // use default destructor
  virtual ~TPCChannelMap() noexcept = default;

  // this is the only function PlaneCoincidence uses
  virtual uint get_plane_from_offline_channel(uint offchannel);

  // going to define the rest of the interface (we can handle the
  // implementation later since it isn't strictly necessary for
  // now)

  // this is pure virtual in DUNE-DAQ but here I think we're only
  // going to need to default implementation wrapping geo::Geom
  virtual uint get_offline_channel_from_crate_slot_fiber_chan(uint crate,
                                                              uint slot,
                                                              uint fiber,
                                                              uint channel) = 0;

  virtual uint get_offline_channel_from_crate_slot_stream_chan(uint crate,
                                                               uint slot,
                                                               uint stream,
                                                               uint channel) = 0;

  // this is the exact default implementation used in detchannelmaps
  virtual std::string get_tpc_element_from_offline_channel(uint) { return ""; }

  virtual std::optional<TPCCoords> get_crate_slot_fiber_chan_from_offline_channel(uint offchannel) = 0;
};

// Makes a channel map given a mapname. Note that in this case the name is
// ignored in favour of the map LArsoft loads, and the argument is kept for
// interface compatibility reasons.
std::shared_ptr<TPCChannelMap> make_map(std::string const &plugin_name);
} // namespace dunedaq::detchannelmaps

#endif