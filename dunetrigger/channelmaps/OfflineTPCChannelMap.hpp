#ifndef DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP
#define DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP

#include <memory>
#include <string>

namespace dunedaq::detchannelmaps {

        class TPCChannelMap {
            public:
                struct TPCCoords
                {
                    uint16_t crate;
                    uint16_t slot;
                    uint16_t fiber;
                    uint16_t channel;
                };

                // use default destructor
                virtual ~TPCChannelMap() noexcept = default;

                // this is the only function PlaneCoincidence uses
                virtual uint get_plane_from_offline_channel(uint offchannel);

        };

        // Makes a channel map given a mapname. Note that in this case the name is ignored in
        // favour of the map LArsoft loads, and the argument is kept for
        // interface compatibility reasons.
       std::shared_ptr<TPCChannelMap> make_map(std::string const& plugin_name);
}


#endif