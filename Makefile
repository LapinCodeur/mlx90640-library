I2C_MODE = LINUX
I2C_LIBS =
#I2C_LIBS = -lbcm2835
SRC_DIR = examples/src/
BUILD_DIR = examples/
LIB_DIR = $(SRC_DIR)lib/

examples = mlxtovideo
examples_objects = $(addsuffix .o,$(addprefix $(SRC_DIR), $(examples)))
examples_output = $(addprefix $(BUILD_DIR), $(examples))

#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
	PREFIX = /usr/local
endif

ifeq ($(I2C_MODE), LINUX)
	I2C_LIBS =
endif

all: libMLX90640_API.a libMLX90640_API.so examples

examples: $(examples_output)

libMLX90640_API.so: functions/MLX90640_API.o functions/MLX90640_$(I2C_MODE)_I2C_Driver.o
	$(CXX) -fPIC -shared $^ -o $@ $(I2C_LIBS)

libMLX90640_API.a: functions/MLX90640_API.o functions/MLX90640_$(I2C_MODE)_I2C_Driver.o
	ar rcs $@ $^
	ranlib $@

functions/MLX90640_API.o functions/MLX90640_RPI_I2C_Driver.o functions/MLX90640_LINUX_I2C_Driver.o : CXXFLAGS+=-fPIC -I headers -shared $(I2C_LIBS)

$(examples_objects) : CXXFLAGS+=-std=c++11

$(examples_output) : CXXFLAGS+=-I. -std=c++11

examples/src/lib/interpolate.o : CC=$(CXX) -std=c++11

examples/src/sdlscale.o : CXXFLAGS+=`sdl2-config --cflags --libs`

$(BUILD_DIR)mlxtovideo: $(SRC_DIR)mlxtovideo.o libMLX90640_API.a
	$(CXX) -L/home/pi/mlx90640-library $^ -o $@ $(I2C_LIBS)

clean:
	rm -f $(examples_output)
	rm -f $(SRC_DIR)*.o
	rm -f $(LIB_DIR)*.o
	rm -f functions/*.o
	rm -f *.o
	rm -f *.so
	rm -f *.a

install: libMLX90640_API.a libMLX90640_API.so
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libMLX90640_API.a $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libMLX90640_API.so $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/include/MLX90640/
	install -m 644 headers/*.h $(DESTDIR)$(PREFIX)/include/MLX90640/
	ldconfig
