# mlx90640-library

This Melexis MLX90640 library's was written for use with the Raspberry Pi and the [MLX90640 breakout](https://shop.pimoroni.com/products/mlx90640-thermal-camera-breakout).


### Generic Linux I2C Mode

Make sure the Linux I2C dev library is installed:

```text
sudo apt-get install libi2c-dev
```

To get the best out of your sensor you should modify `/boot/config.txt` and change your I2C baudrate.

The fastest rate recommended for compatibility with other sensors is 400kHz. This is compatible with SMBus devices:

```text
dtparam=i2c1_baudrate=400000
```

This will give you a framerate of - at most - 8FPS.

If you're just using the MLX90640, you can safely use 1MHz:

```text
dtparam=i2c1_baudrate=1000000
```

This will give you a framerate of - at most - 32FPS.
During my test with a pi4, I did adjust the frequency to

'''text
i2c_arm_baudrate=2000000
'''
To obtain a 1 MHz frequency in output.

Now build the MLX90640 library and examples in LINUX I2C mode:

```text
make clean
make I2C_MODE=LINUX
```

### Dependencies

libav for `video` example:

```text
sudo apt-get install libavutil-dev libavcodec-dev libavformat-dev
sudo apt-get install raspberrypi-kernel-headers
git clone https://github.com/umlaeute/v4l2loopback.git
cd v4l2loopback/
make
sudo make install
sudo depmod -a
```

# Building

After installing the dependencies, you can build the library. Build-modes are:

* `make` or `make all`: build the library and all dependencies.
* `make examples`: only build examples, see below
* `sudo make install`: install libraries and headers into `$PREFIX`, default is `/usr/local`

Afterwards you can run the examples.

# Examples
## mlxtovideo

sudo modprobe v4l2loopback video_nr=3

```
make examples/mlxtovideo
./mlx90640-library-master/examples/mlxtovideo
```

This example translates data from the termal camera to the dev/video0 interface.
