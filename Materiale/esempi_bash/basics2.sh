#!/bin/bash
echo "let's use grep and cut to manipulate files"

[[ ! -e all ]] &&  ./hanoi -flush &> all  # only executes the command if "all" does not exist

echo with \"grep tile all\" we select all lines containing \"tile\" from the file ./all
read -p "..."

grep tile all

echo we can also run \"grep tile\" and feed the input to the stdin of the process. We have 2 ways:

sleep 1

echo 1 \"'grep tile < all'\" connects the stdin of the process with the content of the file ./all

read -p "..."

grep tile < all

sleep 1

echo 2 \"'cat all | grep tile'\" the \| \(pipe\) connects the stdout of \"cat all\" to the stdin of \"grep tile\"

read -p "..."

cat all | grep tile 

echo \"'grep tile < all'\" and \"'cat all | grep tile'\" are equivalent. In this case, \"'grep tile all'\" is equivalent too.
read -p "..."

echo we can chain multiple pipe operators. 
echo for example, can we get the sequence of moved tiles\?
read -p "..."
echo let\'s chain \"cut -d\" \" -f 4\" to get just the 4th word

echo \"'cat all | grep tile | cut -d" " -f 4'\"

read -p "..."

cat all | grep tile | cut -d" " -f 4

sleep 1

echo how to we get rid of the "')'?"

read -p "..."

echo just chain cut again, with -c 1:

echo \"'cat all | grep tile | cut -d" " -f 4 | cut -c 1'\"

read -p "..."

cat all | grep tile | cut -d" " -f 4 | cut -c 1
