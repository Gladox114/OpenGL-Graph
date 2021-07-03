#include <iostream>
#include <vector>


class test {

    public:
        int testVar = 2;
        test(int x)
        {
            testVar = x;
        }

};

int main() {
    
    test *yes[100];
    for (int i = 0; i<100; i++) {
        yes[i] = new test(i);
    }
    for (int i = 0; i<100; i++) {
        std::cout << yes[i]->testVar << std::endl;
    }
    
    
    std::cout << "--------\n";

    std::vector<test*> yes2;

    for (int i = 0; i<100; i++) {
        test* testin = new test(i);
        yes2.push_back(testin);
    }

    for (int i = 0; i<100; i++) {
        std::cout << yes2[i]->testVar << std::endl;
    }

}