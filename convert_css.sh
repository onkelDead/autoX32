#!/bin/bash

cd data
for FILE in `ls *.css` 
do
    NAME=`echo $FILE | cut -d '.' -f1`
    echo $NAME  
    echo "extern \"C\" char ${NAME}_inline_css[];" > ../src/embedded/${NAME}_css.h
    echo "char ${NAME}_inline_css[] = " > ../src/embedded/${NAME}_css.cpp

    cat $FILE | sed "s/\"/\'/g" | sed 's/^/"/g' | sed 's/$/"/g' >> ../src/embedded/${NAME}_css.cpp
    echo ";" >> ../src/embedded/${NAME}_css.cpp

done
cd -
