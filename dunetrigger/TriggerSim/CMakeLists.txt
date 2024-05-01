add_subdirectory(TPAlgTools)
add_subdirectory(TAAlgTools)
add_subdirectory(TCAlgTools)

art_make( BASENAME_ONLY MODULE_LIBRARIES
  dunepdlegacy_Services_ChannelMap_PdspChannelMapService_service
  lardataobj::RawData
  dunecore::DuneObj
  lardata::ArtDataHelper
  nusimdata::SimulationBase	
  ART_FRAMEWORK_CORE
  ART_FRAMEWORK_PRINCIPAL
  ART_FRAMEWORK_SERVICES_REGISTRY
  ART_ROOT_IO_TFILE_SUPPORT
  ROOT::Core
  ART_ROOT_IO_TFILESERVICE_SERVICE
  art::Persistency_Common canvas
  art::Persistency_Provenance canvas
  art::Utilities canvas
  messagefacility::MF_MessageLogger
  cetlib::cetlib cetlib_except
  ROOT_BASIC_LIB_LIST
)


install_fhicl()
install_headers()
install_source()
