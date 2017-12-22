#!/bin/bash

LISTPATH=programList.txt

echo "listing \$PATH files"

echo $PATH | tr ':' '\n' >> temppaths.txt

while read -u 10 p; do
    echo $p >> $LISTPATH
    ls -l $p >> $LISTPATH
    echo "" >> $LISTPATH
    echo "" >> $LISTPATH
done 10<temppaths.txt

rm temppaths.txt
