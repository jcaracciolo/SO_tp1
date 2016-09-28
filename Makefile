all:
	cd Server; make ${ARGS}
	cd Client; make
	rm -f /tmp/*fifo*

clean:
	cd Server; make clean
	cd Client; make clean
	rm -f /tmp/*fifo*
