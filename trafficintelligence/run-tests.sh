#!/bin/sh
echo "------------"
echo "Python tests"
cd python
./run-tests.sh
cd ..
echo "------------"
echo "C++ tests"
if [ -f ./bin/tests ]
then
    ./bin/tests
else
    echo "The test executable has not been compiled"
fi
echo "------------"
echo "Script tests"
./scripts/run-tests.sh
