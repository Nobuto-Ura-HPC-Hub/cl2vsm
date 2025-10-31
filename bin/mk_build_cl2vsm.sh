#!/bin/sh
if [ $# -eq 0 ]; then
SRC_DIR=cl-src
LIST_TXT=${SRC_DIR}/list.txt 
if [ -e ${LIST_TXT} ]; then
    CL_FILES=`grep -v "^#" ${LIST_TXT} | cut -f1 -d\  | xargs`
    SECOND_ARCH=`grep -v "^#" ${LIST_TXT} | sed -e 's/^[^ ][^ ]*$//' -e 's/^[^ ][^ ]* \(.*\)$/\1/' | grep -v asm2 | grep -v llc-sh | xargs -n 1 | sort | uniq`
else
    CL_FILES=`ls -1t ${SRC_DIR}/[a-z]*.cl`
fi
else
SRC_DIR=`dirname $1`
CL_FILES=$*
fi
echo \# $CL_FILES
echo \#

BUILD_DIR=_build
BASE_NAMES=`echo $CL_FILES | xargs -n 1 basename -s .cl`
LL_NAMES=`echo $BASE_NAMES | xargs -n 1 printf "${BUILD_DIR}/%s.ll "`
VSM_NAMES=`echo $BASE_NAMES | xargs -n 1 printf "${BUILD_DIR}/%s.vsm "`
_VSM_NAMES=`echo $BASE_NAMES | xargs -n 1 printf "${BUILD_DIR}/%s._vsm "`

RULE_FILE=/opt/share/mncl-scripts/rule.ninja

cat << NINJA
build cl2vsm: phony ll-mncore vsm param
build ll-mncore: phony ${LL_NAMES}
build vsm: phony ${VSM_NAMES}

rule rm-build-ll-vsm
 command = rm -f $LL_NAMES $VSM_NAMES $_VSM_NAMES
build clean-ll-vsm: rm-build-ll-vsm

#----------------------------------------------------------------
#rule mk_build_cl2vsm
#    command = sh bin/mk_build_cl2vsm.sh > \$out

#build build_cl2vsm.ninja: mk_build_cl2vsm cl2vsm/list.txt

#----------------------------------------------------------------
NINJA

echo -n "build param: phony "
for name in $BASE_NAMES
do
    if [ -e ${SRC_DIR}/${name}.param ];then
        echo -n "${BUILD_DIR}/${name}.param "
    fi
done
echo ""

for name in $BASE_NAMES
do

if [ -e ${SRC_DIR}/${name}.param ];then
    echo build ${BUILD_DIR}/${name}.param: cp ${SRC_DIR}/${name}.param
fi

cat << NINJA2
#----------------------------------------------------------------
build ${BUILD_DIR}/${name}.ll: emit-ll-mncore ${SRC_DIR}/${name}.cl
build ${BUILD_DIR}/${name}._vsm: emit-vsm ${SRC_DIR}/${name}.cl
NINJA2
echo -n "build ${BUILD_DIR}/${name}.vsm: clj-vsm ${BUILD_DIR}/${name}._vsm"
if [ -e ${SRC_DIR}/${name}.param ];then
    echo " | ${BUILD_DIR}/${name}.param"
else
    echo ""
fi


ASM2=`grep -v "^#" ${LIST_TXT} | grep "${name}.*asm2" | cut -f1 -d\ | xargs`
if [ "x$ASM2" != "x" ]; then
    echo build ${BUILD_DIR}/${name}.asm: emit-asm2 ${BUILD_DIR}/${name}.vsm
fi

LLC_SH=`grep -v "^#" ${LIST_TXT} | grep "${name}.*llc-sh" | cut -f1 -d\ | xargs`
if [ "x$LLC_SH" != "x" ]; then
    echo build ${BUILD_DIR}/${name}.sh: generate-llc-sh ${BUILD_DIR}/${name}.ll
fi

echo 

done


for arch in $SECOND_ARCH
do
    FILES=`grep -v "^#" ${LIST_TXT} | grep $arch | cut -f1 -d\ | xargs`
    echo \#----------------------------------------------------------------
    echo \# $arch $FILES
    hit_files=""
    for file in $FILES
    do
        name=`basename -s .cl $file`
        TARGETS="ll asm"
        for target in $TARGETS
        do
            DO_COMMAND=emit-$target-$arch
            if [ $target = "asm" ] ; then
                ext="S"
            else
                ext=$target
            fi
            grep "rule $DO_COMMAND" $RULE_FILE > /dev/null
            if [ $? -eq 0 ]; then
                target_file=${BUILD_DIR}/${name}-${arch}.${ext}
                echo build ${target_file}: $DO_COMMAND ${SRC_DIR}/${name}.cl
                hit_files="$target_file $hit_files"
            fi
        done
    done
    echo
    if [ x"$hit_files" != x ] ; then
        echo build $arch: phony $hit_files
        echo
    fi
done
