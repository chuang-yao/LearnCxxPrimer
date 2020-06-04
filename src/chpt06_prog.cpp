#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::string s1 = argv[1];
    std::string s2 = argv[2];
    std::cout << s1 + s2 << std::endl;
}