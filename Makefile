INC_DIR=/home/arembedded/state_machine_example

SRC_FILES=cam_inf.c \
          cam.c \
          thread1.c \
          $(INC_DIR)/util/util.c \
          main.c 

app: $(SRC_FILES)
	gcc $(SRC_FILES) -I$(INC_DIR) -lpthread -lrt -o app.o

all: app

clean:
	rm -rf *.o	
