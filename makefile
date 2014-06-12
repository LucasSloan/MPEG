jpeg:
	gcc -std=c11 readrgb.o sgitojpeg.c -fopenmp -lm -w -march=native -O3 -o jpeg
reader:
	gcc -std=c11 readrgb.o imagereadertest.c -o reader
sgi:
	gcc -std=c11 -c readrgb.c 
test:
	./jpeg video/07111.sgi test.jpg
clean:
	rm jpeg
	rm reader
	rm readrgb.o
