#!/bin/bash
DIRECTORY=$1
if [[ ! -d "${DIRECTORY}" && ! -L "${DIRECTORY}" ]] ; then
    echo "creating directory ${DIRECTORY}"
    mkdir "${DIRECTORY}"
fi
#if [[ -d "${DIRECTORY}" && ! -L "${DIRECTORY}" ]] ; then
