#!/bin/bash

# simple backup script
# vim:set sw=4 nosta:
#
# Copyright (C) 2003  Tomas Janousek <tomi@nomi.cz>
# See file COPYRIGHT and COPYING

DATE=`date +%Y-%m-%d_%H-%M`
if [ -z "$1" ]; then
    echo "Project name not given"
    exit -1
fi
PROJECT=$1
DIR=`basename \`echo $PWD\``
cd ..
tar -cjf "/backup/$PROJECT""_$DATE.tar.bz2" $DIR
