include(BasicPlugin)

basic_plugin(OfflineTPCChannelMap module
            BASENAME_ONLY
            LIBRARIES
            lardataobj::RawData
            dunecore::DuneObj
            lardata::ArtDataHelper
            nusimdata::SimulationBase	
            ART_FRAMEWORK_CORE
            ART_FRAMEWORK_PRINCIPAL
            ART_FRAMEWORK_SERVICES_REGISTRY
)