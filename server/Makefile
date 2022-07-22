CFLAG+=ljpeg
main:main.c bmpToJpeg.c
	gcc $^ -o $@ -${CFLAG}
	./main
	rm -rf main
clean:
	rm -rf main