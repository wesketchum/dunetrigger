cet_build_plugin(TAAlgTPCExample art::tool
  LIBRARIES
  dunecore::DuneObj
  fhiclcpp 
  cetlib::cetlib 
  cetlib_except
)

cet_build_plugin(TAAlgTPCADCSimpleWindow art::tool
  LIBRARIES
  dunetriggeralgs
  dunecore::DuneObj
  fhiclcpp 
  cetlib::cetlib 
  cetlib_except
)

install_fhicl()
install_headers()
install_source()
