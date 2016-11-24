#!/bin/sh


echo "### TEST 1 (executable) ###"
if : | src/mygzip | grep -q 'X'; then
    echo "OK"
else
    echo "NOT OK"
fi


echo "### TEST 2 (argcount 1) ###"
: | src/mygzip test/t1
echo "OK"
rm test/t1


echo "### TEST 3 (argcount 2, should fail) ###"
if : | src/mygzip test/t2 test/t3 | grep -q 'usage'; then
    echo "OK"
else
    echo "NOT OK"
fi
rm test/t2 test/t3


echo "### TEST 4 (input) ###"
if echo "hello this is input" | src/mygzip | grep -q 'W('; then
    echo "OK"
else
    echo "NOT OK"
fi


echo "### TEST 5 (file) ###"
echo "hello this is input" | src/mygzip test/file.gz
gunzip test/file.gz
if cat test/file | grep -q 'hello this is input'; then
    echo "OK"
    rm test/file
else
    echo "NOT OK"
    rm test/file test/file.gz
fi