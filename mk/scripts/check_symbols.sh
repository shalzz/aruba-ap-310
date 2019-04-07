#!/bin/bash
#
# Copyright (C) 2017 by Aruba Networks, Inc.
# All Rights Reserved.
# 
# This software is an unpublished work and is protected by copyright and 
# trade secret law. Unauthorized copying, redistribution or other use of 
# this work is prohibited.
#
# The above notice of copyright on this source code product does not indicate
# any actual or intended publication of such source code.
#


#
# This file contains a generic symbol-checking script for kernel modules.
# As input, the script takes a single argument that is the path to a file containing a list of ELF files to check, usually vmlinux and *.ko files.
# The script then builds two lists:
#  * the list of all exported symbols.
#  * the list of all required symbols.
# It then verifies that all names from the second list appears in the first list.
# Note that the order of loading is not considered at this time. If a file requires a symbol that is exported anywhere, the test passes.
#
# If a symbol is missing, the script prints an error message and exits with code 1.
# Otherwise, the script exits normally with code 0.

FILELIST="$1"

if [ -z "$FILELIST" ]
then
	echo "Usage: $0 FILELIST"
	echo "Where FILELIST is a file containing paths to ELF files, one path per line."
	exit 2
fi

# Build the lists of exported and required symbols in temporary files
PROVIDED_SYMBOLS=$(mktemp ./.check_symbols_provided_XXXXXX.txt)
REQUIRED_SYMBOLS=$(mktemp ./.check_symbols_required_XXXXXX.txt)

while read -r filename || [[ -n "$filename" ]]
do
	if ! readelf --wide -s "$filename" &>/dev/null
	then
		echo "Warning: $filename is not in ELF format"
	else
		# notes:
		# 1. readelf outputs the symbol table with these columns: Num Value Size Type Bind Vis Ndx Name
		# 2. The first egrep is used to strip out local symbols
		# 3. The first call to awq is used to extract the Bind, Ndx and Name columns
		# 4. The second egrep is used to select the lines that go in each list. If Ndx is UND, then this file does not provide this symbol and requires it instead.
		# 5. The second call to awq is used to keep only the name of the required or provided symbol.
		# 6. The sed call is used to append the name of the providing or requiring file to each line. 
		# as a result, the REQUIRED_SYMBOLS and PROVIDED_SYMBOLS files have two columns: the symbol and the name of the file that requires/provides it.
		readelf --wide -s "$filename" | egrep "GLOBAL"      | awk '{print $5,$7,$8}' | egrep    "^(GLOBAL|WEAK) UND " | awk '{print $3}' | sed "s;\$; $filename;" >>"$REQUIRED_SYMBOLS"
		readelf	--wide -s "$filename" | egrep "GLOBAL|WEAK" | awk '{print $5,$7,$8}' | egrep -v "^(GLOBAL|WEAK) UND " | awk '{print $3}' | sed "s;\$; $filename;" >>"$PROVIDED_SYMBOLS"
	fi
done < "$FILELIST"

FOUND_ERRORS=0

# Check for missing symbols
# loop that reads REQUIRED_SYMBOLS line by line, skipping the empty line at the end.
while read -r required_symbol_entry || [[ -n "$required_symbol_entry" ]]
do
	# split the line to extract the symbol name and the file name.
	required_symbol_name=`echo "$required_symbol_entry" | awk '{print $1}'`
	requiring_file=`echo "$required_symbol_entry" | awk '{print $2}'`
	# check that the symbol is present in the first column of PROVIDED_SYMBOLS 
	if ! awk '{print $1}' "$PROVIDED_SYMBOLS" 2>/dev/null | grep --quiet "$required_symbol_name"
	then
		echo "Error: symbol $required_symbol_name is required by $requiring_file, but this symbol is not exported by any module on this platform."
		FOUND_ERRORS=1
	fi
done < "$REQUIRED_SYMBOLS"

rm -f "$PROVIDED_SYMBOLS" "$REQUIRED_SYMBOLS"

exit $FOUND_ERRORS
