all: opencv
	./opencv > center.txt
opencv: opencv.cpp
	g++ opencv.cpp -o opencv `pkg-config --libs opencv`
compare: compare.cpp
	g++ compare.cpp -o compare `pkg-config --libs opencv`
clean:
	rm -f *.png *.jpg opencv

