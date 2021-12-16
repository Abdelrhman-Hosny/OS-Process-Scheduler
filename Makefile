build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.hpf.c -o scheduler.hpf.out
	gcc scheduler.rr.c -o scheduler.rr.out
	gcc scheduler.srtn.c -o scheduler.srtn.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	echo "start" > proc.txt
	./process_generator.out
