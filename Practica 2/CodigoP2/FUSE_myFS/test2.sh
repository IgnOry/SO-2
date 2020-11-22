rm -R -f tmp
mkdir tmp

rm ./mount-point/polla.txt
cp ./src/fuseLib.c ./tmp
cp ./src/myFS.h ./tmp

cp ./tmp/myFS.h ./mount-point
cp ./tmp/fuseLib.c ./mount-point

diff ./tmp/myFS.h ./mount-point/myFS.h
diff ./tmp/fuseLib.c ./mount-point/fuseLib.c

truncate -o -s -1 ./tmp/myFS.h 
truncate -o -s -1 ./mount-point/myFS.h

diff ./src/myFS.h ./mount-point/myFS.h

cp ./src/fuseLib.h ./mount-point

diff ./src/fuseLib.h ./mount-point/fuseLib.h

truncate -o -s +1 ./tmp/fuseLib.c
truncate -o -s +1 ./mount-point/fuseLib.c

diff ./src/fuseLib.c ./mount-point/fuseLib.c