main_dir=$1
test_dir=$2
dst=$test_dir"dst"

make --directory=$1

echo "Make Real Link from sys fs"

$1/main "/sys/kernel/notes" $dst
code=`echo $?`
files=`ls $test_dir | wc -l`
if [ $code != 0 ] || [ $files != 2 ]; then
    exit 1
fi

if ! cmp -s "/sys/kernel/notes" $dst ; then 
    exit 1
fi


echo "Make Real Link from sys fs existing dst"

$1/main "/sys/kernel/notes" $dst
code=`echo $?`
files=`ls $test_dir | wc -l`
if [ $code != 0 ] || [ $files != 2 ]; then
    exit 1
fi

if ! cmp -s "/sys/kernel/notes" $dst ; then 
    exit 1
fi

rm $dst

exit 0