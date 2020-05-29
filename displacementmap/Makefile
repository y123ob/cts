all: dsmp
	./dsmp small_emily.obj Color_raw/00_diffuse_unlit_raw.exr

exr: exr.cpp
	g++ exr.cpp -o exr `pkg-config --libs opencv`
	./exr

mesh: mesh.cpp
	g++ mesh.cpp -o mesh `pkg-config --libs opencv`

dsmp: displacement_mapping.cpp mesh.cpp mesh.hpp
	g++ mesh.cpp texture.cpp displacement_mapping.cpp -o dsmp `pkg-config --libs opencv`

clean:
	rm -f *.png *.jpg exr compare 

