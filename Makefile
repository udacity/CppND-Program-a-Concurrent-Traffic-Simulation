  
.PHONY: all
all: 
	make clean &&\
	make build &&\
	make run

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	cd build && \
	rm -rf *

.PHONY: run
run:
	cd build && \
	./traffic_simulation
