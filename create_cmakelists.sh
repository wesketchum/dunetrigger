cat << EOF > test.txt
add_compile_options(-O2 -flto -Wno-error=sign-compare -Wno-error=unused-variable)

include(BasicPlugin)

basic_plugin(triggeralgs module
             BASENAME_ONLY
             LIBRARIES
             OfflineTPCChannelMap_module
             SOURCE
             src/TAWindow.cpp
             src/TPWindow.cpp
             src/TriggerActivityMakerADCSimpleWindow.cpp
             src/TriggerActivityMakerBundleN.cpp
             src/TriggerActivityMakerChannelAdjacency.cpp
             src/TriggerActivityMakerChannelDistance.cpp
             src/TriggerActivityMakerDBSCAN.cpp
             src/TriggerActivityMakerHorizontalMuon.cpp
             src/TriggerActivityMakerMichelElectron.cpp
             src/TriggerActivityMakerPlaneCoincidence.cpp
             src/TriggerActivityMakerPrescale.cpp
             src/TriggerActivityMakerSupernova.cpp
             src/TriggerCandidateMakerADCSimpleWindow.cpp
             src/TriggerCandidateMakerBundleN.cpp
             src/TriggerCandidateMakerChannelAdjacency.cpp
             src/TriggerCandidateMakerChannelDistance.cpp
             src/TriggerCandidateMakerDBSCAN.cpp
             src/TriggerCandidateMakerHorizontalMuon.cpp
             src/TriggerCandidateMakerMichelElectron.cpp
             src/TriggerCandidateMakerPlaneCoincidence.cpp
             src/TriggerCandidateMakerPrescale.cpp
             src/TriggerCandidateMakerSupernova.cpp
             src/TriggerDecisionMakerSupernova.cpp
             src/dbscan/dbscan.cpp
             src/dbscan/Hit.cpp
)

# TODO PAR 2021-04-15: What is in autogen? Is it actually used?
add_subdirectory(autogen)
EOF

