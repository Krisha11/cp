main_dir=$1
test_dir=$2
dst=$test_dir"dst"
src1=$test_dir"src1"
src2=$test_dir"src2"
src3=$test_dir"src3"

make --directory=$1

touch $src1
ln -s $src1 $src2 # $src2 -> $src1
ln -s $src2 $src3 # $src2 -> $src1

echo "Make Soft Link"

$1/main $src3 $dst
code=`echo $?`
files=`ls $test_dir | wc -l`

src3_link=`readlink $src3`
src2_link=`readlink $src2`
dst_link=`readlink $dst`

if [ $code != 0 ] || [ $files != 5 ] || [ $src3_link != $dst_link ] || [ $src3_link == $src2_link ] ; then
    exit 1
fi

rm $src1
rm $src2
rm $src3
rm $dst
