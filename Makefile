all: opencv
	./opencv
opencv: opencv.cpp
	g++ opencv.cpp -o opencv `pkg-config --libs opencv`
clean:
	rm -f sep_* albedo_from_diffuse.jpg normal_from_diffuse.jpg opencv

