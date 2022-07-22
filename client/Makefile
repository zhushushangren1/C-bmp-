CFLAG+=ljpeg
main:main.c jpegToBmp.c
	gcc $^ -o $@ -${CFLAG}
	./main
	rm -rf main