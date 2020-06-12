# audio-recorder
Linux audio recorder using GStreamer 

https://gstreamer.freedesktop.org/documentation/tools/gst-launch.html?gi-language=c 

Records 10 seconds of audio data to file from the default input device on the pi.  Tested on Raspberry Pi OS with pi zero w.

Assumes setup of:

```console
sudo apt-get install gstreamer1.0-tools

sudo apt-get install libgstreamer1.0-dev

pkg-config --cflags --libs gstreamer-1.0
```

You may have to check audio device and ensure you have the right input device for recording:


```console
cat /proc/asound/cards
```

and then edit /etc/asound.conf with following:

```console
   defaults.pcm.card 1
   defaults.ctl.card 1
```
Replace "1" with number of your card determined above.

Could run the following to see if GStreamer is able to start a pipeline:

```console
gst-launch-1.0 -e -v alsasrc !  audio/x-raw,rate=8000 ! wavenc ! filesink location=output.wav
```

Changing default device can be done - follow steps here:
https://superuser.com/questions/626606/how-to-make-alsa-pick-a-preferred-sound-device-automatically


Compile and run:

```
gcc audio-record.c -o audio-record.o `pkg-config --cflags --libs gstreamer-1.0`
./audio-record.o
```



