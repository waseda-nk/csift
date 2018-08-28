#!/bin/sh

# This program run Structure from Motion
#  made by Nobuyuki Koizumi.
#  $1 ... image directory
#  $2 ... result directory


# Detect Directory
echo -en "[\033[0;32mRunSfM\033[0;39m] Detect Directory --- "
if [ $# -ne 2 ]; then 
  echo -e "[\033[0;31mError\033[0;39m]"
  echo -e "  Directory NOT Found." 1>&2
  echo -e "  \`sh ./RunSfM.sh <image-dir> <result-dir>\`" 1>&2
  exit 1
else 
  echo -e "[\033[0;32mOK\033[0;39m]"
  ImageDir=`readlink -f ${1}`
  ResultDir=`readlink -f ${2}`
  CurrentDir=`pwd`
  cat << EOH
  Image Directory
    ${ImageDir}
  Result Directory
    ${ResultDir}
EOH
fi


# Write Image List
echo -en "[\033[0;32mRunSfM\033[0;39m] Write Image List --- "
ImageList="${ResultDir}/ImageList.txt"
KeypointList="${ResultDir}/KeypointList.txt"
rm -f "${ImageList}" "${KeypointList}"
find "${ImageDir}" -maxdepth 1 -name '*.jpg' | sort > "${ImageList}"

if [ ! -s "${ImageList}" ]; then
  echo -e "[\033[0;31mError\033[0;39m]"
  echo -e "  JPG file NOT Found." 1>&2
  exit 1
else
  echo -e "[\033[0;32mOK\033[0;39m]"
fi


# Run SIFT
echo -en "[\033[0;32mRunSfM\033[0;39m] Run SIFT --- "
SIFT=${CurrentDir}/sift
if [  -e "${SIFT}" ] ; then
  echo -e "[\033[0;31mError\033[0;39m]"
  echo -e "    openSIFT NOT Found." 1>&2
  echo -e "    Install SIFT to Current Directory" 1>&2
  echo -e "      ${CurrentDir}" 1>&2
  exit 1
fi

cat "${ImageList}" | while read line
do
  pgm=${line%.jpg}.pgm
  key=${line%.jpg}.key
  echo ${key} >> ${KeypointList}
<<COMMENT_OUT
  /usr/bin/mogrify -format pgm "${file}"
  "${SIFT}" < "${pgm}" > "${key}"
  gzip -f "${key}"
  rm "${pgm}"
COMMENT_OUT
done

#KeyMatchFull "${ResultDir}/list_keys.txt" "${ResultDir}/matches.init.txt" -1
echo -e "[\033[0;32mOK\033[0;39m]"


# Run SfM
