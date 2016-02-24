#!/bin/sh
if [ $# -ge 1 ];
then
    installDirname=$1
    echo 'Removing from directory '$installDirname
else
    installDirname='/usr/local/bin'
    echo 'Removing from default directory '$installDirname
fi

for filename in *
do
    echo 'rm '$installDirname/$filename
    rm $installDirname/$filename
done
