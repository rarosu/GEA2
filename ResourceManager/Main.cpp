#include <functional>
#include <string>
#include <iostream>

int main()
{
	std::string s = "Hello world";
	std::hash<std::string> hf;
	size_t h = hf(s);

	std::cout << h << std::endl;
}