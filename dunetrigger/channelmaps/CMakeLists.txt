cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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