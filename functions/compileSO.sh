
for file in ./*.cc
do
    g++ ./$file ../glad.c -o ./${file%??}so -shared -fPIC
done
