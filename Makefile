all:
	g++ -std=c++17 main.cpp -o main src/file_descriptor.cpp src/copier.cpp -lstdc++fs -I src/
