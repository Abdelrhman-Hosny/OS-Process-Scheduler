build:
	./scripts/create_out.sh
	gcc process_generator.c -o ./out/process_generator.out -lm
	gcc clk.c -o ./out/clk.out -lm
	gcc scheduler.hpf.c -o ./out/scheduler.hpf.out -lm
	gcc scheduler.rr.c -o ./out/scheduler.rr.out -lm
	gcc scheduler.srtn.c -o ./out/scheduler.srtn.out -lm
	gcc process.c -o ./out/process.out -lm
	gcc test_generator.c -o test_generator.out

clean:
	./scripts/clear_files.sh
	rm -rf out *.out processes.txt

all: clean build

run:
	./scripts/clear_files.sh
	./out/process_generator.out
