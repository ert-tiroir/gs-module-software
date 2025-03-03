
core:
	g++ -o AV_Core \
		`find av-core -name \*.cpp` \
		`find av-module -name \*.cpp` \
		`find utils -name \*.cpp` \
		communication/communication/buffer.cpp \
		communication/communication/rpi_spi.c \
		-I./ -I./communication -lwiringPi -lpthread -lm -lcrypt
camera:
	g++ -o GS_Camera_Module \
	    `find av-module -name \*.cpp`\
	    `find gs-camera -name \*.cpp`\
	    `find utils -name \*.cpp` \
		-I./ -I./communication
