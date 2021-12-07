#!/bin/bash

cd data
for FILE in `ls *.glade` 
do
    NAME=`echo $FILE | cut -d '.' -f1`
    echo $NAME  
    echo "extern \"C\" char ${NAME}_inline_glade[];" > ../src/embedded/${NAME}.h
    echo "char ${NAME}_inline_glade[] = " > ../src/embedded/${NAME}.cpp
    cat $FILE | sed "s/\"/\'/g" | sed 's/^/"/g' | sed 's/$/"/g' >> ../src/embedded/${NAME}.cpp
    echo ";" >> ../src/embedded/${NAME}.cpp

done
cd -
