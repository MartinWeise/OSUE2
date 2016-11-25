#!/bin/sh

# @file test.sh
# @author Martin Weise <e1429167@student.tuwien.ac.at>
# @date 25.11.2016
#
# @brief Contains 6 simple testcases to quick-check the program
#        against the task description.

echo "### TEST 1 (executable) ###"
if : | src/mygzip | grep -q 'X'; then
    echo "OK"
else
    echo "NOT OK"
fi


echo "### TEST 2 (argcount 1) ###"
if echo "Hello waddup" | src/mygzip test/t1.gz | grep -q ''; then
    gunzip test/t1.gz
    if cat test/t1 | grep -q 'Hello waddup'; then
        echo "OK"
    else
        echo "NOT OK 2"
    fi
else
    echo "NOT OK"
fi
rm test/t1


echo "### TEST 3 (argcount 2) ###"
#                                 vv pipe sterr to stdout & stdout to /dev/null
if : | src/mygzip test/t2 test/t3 2>&1 >/dev/null | grep -q 'USAGE'; then
    echo "OK"
else
    rm test/t2 test/t3
    echo "NOT OK"
fi


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

echo "#### TEST 6 (stdout) ###"
(echo "waddup" | src/mygzip) > output.gz
gunzip output.gz
if !(cat output | grep -q 'waddup'); then
    echo "NOT OK"
else
    echo "OK"
fi
rm output