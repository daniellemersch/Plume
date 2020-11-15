#!/bin/bash
# Written by Alessandro Crespi
# Creates a temporary file with a do-nothing C program

tmp=/tmp/testlibpng.c
cat <<EOF >$tmp
int main(){
  return 0;
}
EOF

# Tries to compile the program linking it to libpng
gcc -o /dev/null $tmp -lpng 2>/dev/null

# Takes the return code of gcc (0 = no error)
ret=$?

# Erases the temporary source code
rm -f $tmp

# Writes the -l options for the compiler, if the libraries are available
if [ $ret -eq 0 ]; then
  echo  -lpng -ltiff
fi

# Returns the exit code of gcc
exit $ret
