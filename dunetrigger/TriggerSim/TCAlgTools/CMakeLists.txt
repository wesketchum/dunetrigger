cet_build_plugin(TCAlgTPCExample art::tool
  LIBRARIES
  dunecore::DuneObj
  fhiclcpp 
  cetlib::cetlib 
  cetlib_except
)

install_fhicl()
install_headers()
install_source()
