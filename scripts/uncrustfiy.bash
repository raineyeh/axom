#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
UNCRUSTIFY_EXEC=$DIR/../src/TPL/uncrustify/src/uncrustify

for file in $(find . -type f -regex ".*\.\(hpp\|cpp\)" -follow -print0 | xargs -0); do
    $UNCRUSTIFY_EXEC -c uncrustify.cfg --no-backup $file
    echo $file
done

    
#echo $FILES
