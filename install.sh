#!/bin/sh

echo "........................."
echo "Installing minisat"
echo "........................."


# sudo apt install curl
# sudo apt install zlib1g-dev

mkdir solver

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
curl -sS https://www.labri.fr/perso/lsimon/downloads/softwares/glucose-syrup-4.1.tgz > glucose.tgz
tar -xvzf glucose.tgz
rm glucose.tgz
cd glucose-syrup-4.1/simp
make rs

echo "........................."
echo "Compiling AAN-reach"
echo "........................."

cd ../../..
mkdir obj
mkdir obj/asp
mkdir obj/encoding
mkdir obj/interface
mkdir obj/lcg
mkdir obj/logic
mkdir obj/model

make aan_reach
