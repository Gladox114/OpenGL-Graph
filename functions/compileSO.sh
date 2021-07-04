
for file in ./*.cc
do
    g++ ./$file -o ./${file%??}so -shared -fPIC
done
