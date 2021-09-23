#!/bin/bash

# this is a comment
# execute the script with either of these:
# bash basics.sh
# /bin/bash basics.sh
# ./basics.sh
# for the last one, basics.sh must be executable (set with "chmod +x basics.sh")
# WSL users: you may need to run "dos2unix script.sh" if script.sh is giving you "$'\r': command not found"

echo I am the script $0

echo or, more nicely, I am $(basename $0)

echo we run \"'./hanoi'\" which solves the hanoi tower puzzle, all output prints on terminal

read -p "(press enter to continue...)"

./hanoi 

sleep 1
# clear

echo \"'./hanoi > out'\" redirects stdout to ./out, strerr prints on terminal

read -p "..."
./hanoi > out

sleep 1
echo see\? we only see stderr

sleep 1

echo \"'./hanoi > out 2> err'\" redirects stdout to ./out and stderr to ./err

read -p "..."

./hanoi > out 2> err

sleep 1
echo as expected, we do not see anything 
echo "(NOTE: the > and 2> operators OVERWRITE existing files. We can append instead with >> and 2>>)"

sleep 1

echo but we can see it went to the files with \"head -n 3 out err\"
read -p "..."

head -n 3 out err

sleep 1


echo \"'./hanoi &> all'\" redirects both strout and stderr to ./all \(beware\)

read -p "..."

./hanoi &> all

sleep 1

echo let\'s have a look at ./all with \"cat all\":

read -p "..."

cat all

echo why does it look bad\?

read -p "..."

echo \"'./hanoi -flush &> all'\" flushes the streams manually, let\'s run it

./hanoi -flush &> all

sleep 1 
echo let\'s have another look at ./all with \"cat all\":


read -p "..."


cat all