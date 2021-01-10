# !/bin/bash

MPOINT="./mount-point"
TEMP="./temp"

rmdir $TEMP
mkdir $TEMP

cp ./src/fuseLib.c $MPOINT/
cp ./src/fuseLib.c $TEMP/
cp ./src/myFS.h $MPOINT/
cp ./src/myFS.h $TEMP/

./my-fsck virtual-disk

DIFF_1=$(diff $MPOINT/fuseLib.c $TEMP/fuseLib.c)
DIFF_2=$(diff $MPOINT/myFS.h $TEMP/myFS.h)

if ["$DIFF_1" == ""]
then echo "[FIRST FILE EQUAL]"
else echo "[FIRST FILE NOT EQUAL]"
fi

if ["$DIFF_2" == ""]
then echo "[SECOND FILE EQUAL]"
else echo "[SECOND FILE NOT EQUAL]"
fi

truncate --size=-4096 $MPOINT/fuseLib.c
truncate --size=-4096 $TEMP/fuseLib.c

./my-fsck virtual-disk

DIFF_TRUNC=$(diff ./src/fuseLib.c $MPOINT/fuseLib.c)

if ["$DIFF_TRUNC" == ""]
then echo "[TRUNCATED FILE EQUAL]"
else echo "[TRUNCATED FILE NOT EQUAL]"
fi

cp ./src/MyFileSystem.c $MPOINT/

./my-fsck virtual-disk

DIFF_3=$(diff ./src/MyFileSystem.c $MPOINT/MyFileSystem.c)

if ["$DIFF_3" == ""]
then echo "[THIRD FILE EQUAL]"
else echo "[THIRD FILE NOT EQUAL]"
fi

truncate --size=+8192 $MPOINT/fuseLib.c

./my-fsck virtual-disk

DIFF_4=$(diff ./src/MyFileSystem.c $MPOINT/MyFileSystem.c)

if ["$DIFF_4" == ""]
then echo "[FOURTH FILE EQUAL]"
else echo "[FOURTH FILE NOT EQUAL]"
fi

unlink $MPOINT/fuseLib.c

echo "[FIRST FILE DELETED]"