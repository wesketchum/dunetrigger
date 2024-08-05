#ifndef DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP
#define DUNETRIGGER_CHANNELMAPS_OFFLINETPCCHANNELMAP_HPP

#include <memory>
#include <string>

namespace dunedaq {
    namespace detchannelmaps {
        class TPCChannelMap {
            public:
                struct TPCCoords
                {
                    uint16_t crate;
                    uint16_t slot;
                    uint16_t fiber;
                    uint16_t channel;
                };

                virtual ~TPCChannelMap() noexcept = default;
                virtual uint get_plane_from_offline_channel(uint offchannel) = 0;

        };

        class VDColdboxChannelMap : public TPCChannelMap {
            public:
                uint get_plane_from_offline_channel(uint offchannel) override;
        };

       std::shared_ptr<TPCChannelMap> make_map(std::string const& plugin_name);

    }
}


#endif