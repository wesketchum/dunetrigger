#!/bin/bash

path="dunetrigger/triggeralgs"
directories=("$path/src" "$path/include/triggeralgs")

old_text="\"triggeralgs/"
new_text="\"dunetrigger/triggeralgs/include/triggeralgs/"

old_text_1="\"trgdataformats/"
new_text_1="\"detdataformats/trigger/"

old_text_2="\"detchannelmaps/TPCChannelMap.hpp"
new_text_2="\"dunetrigger/channelmaps/OfflineTPCChannelMap.hpp"

# Loop on each directory
for directory in "${directories[@]}"; do
    find "$directory" -type f \( -name "*.hxx" -o -name "*hpp" -o -name "*.cpp" \) | while read -r file; do

	if grep -q "$new_text" "$file"; then
	    echo "no need of text replacement: $new_text"
	else sed -i "s?$old_text?$new_text?g" "$file"
	fi

        if grep -q "$new_text_1" "$file"; then
            echo "no need of text replacement: $new_text_1"
        else sed -i "s?$old_text_1?$new_text_1?g" "$file"
        fi

        if grep -q "$new_text_2" "$file"; then
            echo "no need of text replacement: $new_text_2"
        else sed -i "s?$old_text_2?$new_text_2?g" "$file"
        fi

	# comment out if not already done
        sed -i '/logging\/Logging\.hpp/{/^\/\//!s/^/\/\/ /;}' "$file"
	sed -i '/ers\/Issue\.hpp/{/^\/\//!s/^/\/\/ /;}' "$file"
        sed -i '/FactoryOverwrite/{/^\/\//!s/^/\/\/ /;}' "$file"
        sed -i '/FactoryNotFound/{/^\/\//!s/^/\/\/ /;}' "$file"
        sed -i '/BadConfiguration/{/^\/\//!s/^/\/\/ /;}' "$file"
	sed -i '/ERS_DECLARE_ISSUE/{/^\/\//!s/^/\/\/ /; n; /^\/\//!s/^/\/\/ /; n; /^\/\//!s/^/\/\/ /; n; /^\/\//!s/^/\/\/ /;}' "$file"

    done
done

# one last commenting out
sed -i "/TLOG()/{/^\/\//!s/^/\/\/ /;}" "$path/include/triggeralgs/AbstractFactory.hxx"

