CC=gcc
FFMPEG_PREF=/opt/homebrew/Cellar/ffmpeg/8.1.2
CFLAGS=-c -g -Wall -I$(FFMPEG_PREF)/include
LDFLAGS=-L$(FFMPEG_PREF)/lib -lavcodec -lavformat -lavutil -lswscale
TARGET=bin/

all: prepare motion_detector

prepare: 
	mkdir -p $(TARGET)

motion_detector: main.o queue.o frame_handler.o video_handler.o
	$(CC)  $(TARGET)main.o $(TARGET)queue.o $(TARGET)frame_handler.o $(TARGET)video_handler.o $(LDFLAGS) -o $(TARGET)motion_detector

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c -o $(TARGET)main.o

queue.o: src/queue.c
	$(CC) $(CFLAGS) src/queue.c -o $(TARGET)queue.o

frame_handler.o: src/frame_handler.c
	$(CC) $(CFLAGS) src/frame_handler.c -o $(TARGET)frame_handler.o

video_handler.o: src/video_handler.c
	$(CC) $(CFLAGS) src/video_handler.c -o $(TARGET)video_handler.o

clean:
	rm -rf $(TARGET)*.o $(TARGET)main
