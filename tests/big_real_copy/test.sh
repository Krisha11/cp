main_dir=$1
test_dir=$2

make --directory=$1
echo "Make Real Link from sys fs"

file=/dev/shm/temp_file
head -c 100 /dev/urandom > $file

$main_dir/main $file $test_dir"dst"
code=`echo $?`
files=`ls $2 | wc -l`
if [ $code != 0 ] || [ $files != 2 ]; then
    exit 1
fi

if ! cmp -s $file $test_dir"dst" ; then 
    exit 1
fi

rm $test_dir"dst"
rm $file


exit 0