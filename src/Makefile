all:
	gcc -o trainer `pkg-config --cflags --libs glib-2.0` -I../include trainer.c ../lib/libstemmer.o `pkg-config --libs glib-2.0`
	gcc -o predictor `pkg-config --cflags --libs glib-2.0` -I../include predictor.c ../lib/libstemmer.o `pkg-config --libs glib-2.0` -lm
