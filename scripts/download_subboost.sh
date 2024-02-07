#!/bin/bash

WHERE_TO_PLACE=contrib/subset-boost
WHOLE_BOOST="${HOME}/.cmake.cache/boost"
BOOST_SUBVER=$2
[[ -z "$BOOST_SUBVER" ]] && BOOST_SUBVER=84
BOOST_COMPONENTS="signals2"
BOOST_DIR=boost_1_${BOOST_SUBVER}_0
BOOST_FILE_ARCH=${BOOST_DIR}.tar.gz
DOWNLOAD_URL=https://boostorg.jfrog.io/artifactory/main/release/1.${BOOST_SUBVER}.0/source/${BOOST_FILE_ARCH}

if ! [ -d ${WHERE_TO_PLACE} ]; then
    [ -f ${WHOLE_BOOST}/${BOOST_FILE_ARCH} ] || wget ${DOWNLOAD_URL} -P ${WHOLE_BOOST}
    [ -d ${WHOLE_BOOST}/${BOOST_DIR} ] || tar -xvf ${WHOLE_BOOST}/${BOOST_FILE_ARCH} -C ${WHOLE_BOOST}
fi

# check if the needed cmake file exists
if ! [ -f $1 ]; then
    CUR=`pwd`
    cd ${WHOLE_BOOST}/${BOOST_DIR}
    SUBBOOST=${CUR}/${WHERE_TO_PLACE}
    mkdir -p ${SUBBOOST}
    # extract needed subcomponents from boost library
    bcp build ${BOOST_COMPONENTS} ${SUBBOOST}
    cd ${SUBBOOST}
    ./bootstrap.sh
    ./b2
fi
