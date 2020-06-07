all: dsmp
	./dsmp

prep: geometry.o mesh.o texture.o util.o preprocess.o
	g++ geometry.o mesh.o texture.o util.o preprocess.o -o prep `pkg-config --libs opencv`
	./prep Meshes/tri_emily.obj

preprocess.o: preprocess.cpp
	g++ -c preprocess.cpp `pkg-config --libs opencv`

mesh.o: mesh.cpp
	g++ -c mesh.cpp `pkg-config --libs opencv`

util.o: util.cpp
	g++ -c util.cpp `pkg-config --libs opencv`

geometry.o: geometry.cpp
	g++ -c geometry.cpp `pkg-config --libs opencv`

texture.o: texture.cpp
	g++ -c texture.cpp `pkg-config --libs opencv`

displacement_mapping.o: displacement_mapping.cpp
	g++ -c displacement_mapping.cpp `pkg-config --libs opencv`

dsmp: mesh.o util.o geometry.o texture.o displacement_mapping.o
	g++ -o dsmp geometry.o mesh.o util.o texture.o displacement_mapping.o `pkg-config --libs opencv`

clean:
	rm -f *.png *.jpg exr *.o dsmp prep face

