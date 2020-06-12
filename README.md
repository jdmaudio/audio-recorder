# audio-recorder
Linux audio recorder using GStreamer 

https://gstreamer.freedesktop.org/documentation/tools/gst-launch.html?gi-language=c 

Records audio to file from the default input device. Tested on pi zero w running Raspberry Pi OS.

Assumes setup of:

```console
sudo apt-get install gstreamer1.0-tools

sudo apt-get install libgstreamer1.0-dev

pkg-config --cflags --libs gstreamer-1.0
```

You may have to check right input audio device is selected  e.g. from USB:


```console
cat /proc/asound/cards
```

and then edit /etc/asound.conf with following:

```console
defaults.pcm.card 1
defaults.ctl.card 1
```
Replace "1" with number of your card determined above.

Or changing default device can be done following steps here:
https://superuser.com/questions/626606/how-to-make-alsa-pick-a-preferred-sound-device-automatically


Once the device is setup you could run the following to see if GStreamer is able to start a pipeline and record data to file:

```console
gst-launch-1.0 -e -v alsasrc !  audio/x-raw,rate=8000 ! wavenc ! filesink location=output.wav
```

Compile and run:

```console
gcc audio-record.c -o audio-record.o `pkg-config --cflags --libs gstreamer-1.0`
./audio-record.o
```



