main_dir=$1
test_dir=$2

make --directory=$1

echo "Try copy non existing arguments"
$main_dir/main nonExistSrc nonExistDst
code=`echo $?`
files=`ls $test_dir | wc -l`
if [ $code != 1 ] || [ $files != 1 ]; then
    exit 1
fi


echo "Try copy directory"
mkdir $test_dir"DirSrc"
$1/main $test_dir"DirSrc" $test_dir"nonExistDst"
code=`echo $?`
files=`ls $test_dir | wc -l`
if [ $code != 1 ] || [ $files != 2 ]; then
    exit 1
fi
rm -r $test_dir"DirSrc"

exit 0