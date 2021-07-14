
for file in ./*.cc
do
    g++ ./$file ../glad.c ../configReadTest.cpp -o ./${file%??}so -shared -fPIC
done
