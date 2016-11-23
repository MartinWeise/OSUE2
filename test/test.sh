#!/bin/sh

echo "### TEST 1 (executable) ###"
: | src/mygzip
echo "OK"

echo "### TEST 2 (argcount 1) ###"
[[ $(src/mygzip t1 t2) =~ "USAGE" ]] || echo "NOT OK"

echo "### TEST 3 (argcount 2, should fail) ###"
[[ $(src/mygzip t1 t2) =~ "USAGE" ]] || echo "NOT OK"

echo "### TEST 2 (input) ###"
echo "hello this is input" | src/mygzip
echo "OK"

echo "### TEST 3 (file) ###"
echo "hello this is input" | src/mygzip test/file
if test -e "test/file";
then
    echo "OK"
else
    echo "NOT OK: No file named file was created.\n"
fi
rm test/file