# Simple motion detector

It's simple motion detector project which uses ffmpeg to process video

## The only feature
Detects motions

## Config
Before building make sure you have pre installed ffmpeg, and paste it's path to Makefile. 
Here:
<img width="819" height="108" alt="Screenshot 2026-07-11 at 12 35 51" src="https://github.com/user-attachments/assets/ba0e6a28-951b-4c88-b3fc-9a53d37bc075" />

## Build
```
cd MotionDetector
make
```
or
```
cd MotionDetector
bear -- make
```

## Run
```
./bin/motion_detector <video_file_path>
```
<img width="1076" height="216" alt="Screenshot 2026-07-11 at 12 49 31" src="https://github.com/user-attachments/assets/92509fdc-2ee8-428c-856c-e5a711d1e0d9" />
