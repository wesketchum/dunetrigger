#!/bin/bash

# Creating file
touch CMakeLists_tmp.txt

# if the file was created
if [ -f "CMakeLists_tmp.txt" ]; then
    
    # Add the content to the CMakeLists_tmp.txt file
    echo 'add_compile_options(-O2 -flto -Wno-error=sign-compare -Wno-error=unused-variable)' > CMakeLists_tmp.txt
    echo '' >> CMakeLists_tmp.txt
    echo 'include(BasicPlugin)' >> CMakeLists_tmp.txt
    echo '' >> CMakeLists_tmp.txt
    echo 'basic_plugin(triggeralgs module' >> CMakeLists_tmp.txt
    echo '             BASENAME_ONLY' >> CMakeLists_tmp.txt
    echo '             LIBRARIES' >> CMakeLists_tmp.txt
    echo '             OfflineTPCChannelMap_module' >> CMakeLists_tmp.txt
    echo '             SOURCE' >> CMakeLists_tmp.txt
    for file in dunetrigger/triggeralgs/src/*.cpp; do
        echo "             ${file#dunetrigger/triggeralgs/}" >> CMakeLists_tmp.txt
    done
    for file in dunetrigger/triggeralgs/src/*/*.cpp; do
	echo "             ${file#dunetrigger/triggeralgs/}" >> CMakeLists_tmp.txt
    done
    echo ')' >> CMakeLists_tmp.txt
    echo '' >> CMakeLists_tmp.txt
    echo '# TODO PAR 2021-04-15: What is in autogen? Is it actually used?' >> CMakeLists_tmp.txt
    echo 'add_subdirectory(autogen)' >> CMakeLists_tmp.txt

    mv CMakeLists_tmp.txt dunetrigger/triggeralgs/CMakeLists.txt
fi
