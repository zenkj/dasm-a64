MINILUA = ../../bin/minilua
DASM = ../../src/dynasm.lua

test: bf bfjit
	@echo normal bf interpreter running...
	@time ./bf mandelbrot.bf
	@echo
	@echo jit running...
	@time ./bfjit mandelbrot.bf
bf: bf.c
	gcc -g -O2 -o $@ $<
bfjit: bfjit.c
	gcc -g -O2 -o $@ $<

bfjit.c: bf_a64.dasc $(MINILUA)
	$(MINILUA) $(DASM) -o $@ $< 

dump: bf_a64.dasc $(MINILUA)
	$(MINILUA) $(DASM) --dumparch a64 $< >dump.a64

$(MINILUA): ../../dep/minilua/minilua.c
	make -C ../../dep/minilua

clean:
	rm -f bf bfjit bfjit.c dump.a64
