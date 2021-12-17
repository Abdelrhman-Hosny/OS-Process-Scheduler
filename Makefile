build:
	gcc process_generator.c -o process_generator.out -lm
	gcc clk.c -o clk.out -lm
	gcc scheduler.hpf.c -o scheduler.hpf.out -lm
	gcc scheduler.rr.c -o scheduler.rr.out -lm
	gcc scheduler.srtn.c -o scheduler.srtn.out -lm
	gcc process.c -o process.out -lm
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	echo "# clean file" > scheduler.log
	echo "# clean file" > scheduler.perf
	./process_generator.out
