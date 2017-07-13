#! /bin/bash

#
#  Function to create a directory and (optionally) any of it's parents.
#
#  The purpose of this function (and script) is to allow one or
#  more processes to create directory trees where they may share the
#  same parents. On some platforms, the "mkdir -p" will return an error
#  if it begins to create the parent directory, only to find out another
#  process did this first. To prevent this from happening, the directory
#  tree will be created manually, checking for errors along the way. If
#  the mkdir command fails, the error is returned iff the directory was
#  not created. Since the mkdir command wouldn't have been executed if 
#  the directory existed in the first place, this should prevent the 
#  false error.
#
createDir() {
    myArgs=$1
    createParent=$2
    myDir=$3

    status=0
    if [ -d "$myDir" ]; then
	return $status
    fi

    fullDir=""
    if [ $createParent -ne 0 ]; then
	for subDir in `echo $myDir | sed -e 's:\(.\)/:\1 :g'`; do
	    if [ "$fullDir" = "" ]; then
		fullDir="$subDir"
	    else
		fullDir="$fullDir/$subDir"
	    fi

	    if [ ! -e "$fullDir" ]; then
		res=`mkdir $myArgs $fullDir 2>&1`
		status=$?
		if [ $status -ne 0 -a ! -e "$fullDir" ]; then
		    echo $res >&2
		    return $status
		fi
		status=0
	    fi
	done
    else
	res=`mkdir $myArgs $myDir 2>&1`
	status=$?
    fi

    if [ $status -ne 0 -a ! -d "$myDir" ]; then
	echo $res >&2
	return $status
    fi

    return 0
}


#
#  Pull any of the optional arguments off the command line and store for later use
#
args=""
createParents=0
while [ -n "$1" ]; do
    case "$1" in
    -p)		createParents=1; shift 1;;
    -m)		args="$args -m $2"; shift 2;;
    *)		break;;
    esac
done

#
#  The loop above will exit after parsing -p and -m and $1 should be pointing to the
#  first directory on the list.
#

exitStatus=0
while [ -n "$1" ]; do
    dir="$1"; shift;
    createDir "$args" "$createParents" "$dir"
    status=$?
    exitStatus=`expr $exitStatus \| $status`
done

exit $exitStatus

