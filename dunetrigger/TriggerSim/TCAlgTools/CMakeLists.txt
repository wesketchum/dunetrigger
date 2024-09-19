cet_build_plugin(TCAlgTPCExample art::tool
  LIBRARIES
  dunecore::DuneObj
  fhiclcpp 
  cetlib::cetlib 
  cetlib_except
)

cet_build_plugin(TCAlgTPCOnline art::tool
  LIBRARIES
  fhiclcpp
  cetlib::cetlib
  cetlib_except
)

install_fhicl()
install_headers()
install_source()
