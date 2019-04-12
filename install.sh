#!/bin/sh

echo "........................."
echo "Installing minisat"
echo "........................."


# sudo apt install curl
# sudo apt install zlib1g-dev

curl -sS http://minisat.se/downloads/minisat-2.2.0.tar.gz > solver/minisat.tar.gz
cd solver
tar -xvzf minisat.tar.gz
rm minisat.tar.gz
cd minisat
export MROOT="${PWD}"
cd core
make

echo "........................."
echo "Installing glucose"
echo "........................."

cd ../..
curl -sS http://www.labri.fr/perso/lsimon/downloads/softwares/glucose-syrup-4.1.tgz > glucose.tar.gz
tar -xvzf glucose.tar.gz
rm glucose.tar.gz
cd glucose-syrup-4.1/simp
make rs

echo "........................."
echo "Compiling aan_reach"
echo "........................."

cd ../../..
make aan_reach
