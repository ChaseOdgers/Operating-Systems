all: filter map reduce fmr

fmr: fmr.c
	gcc -Wall -D LABTASK $^ -o $@
	
bonus: filter map reduce fmr.c
	gcc -Wall -D BONUS $^ -o fmr

filter: filter.c
	gcc -Wall $^ -o $@
	
map: map.c
	gcc -Wall $^ -o $@ -lm
	
reduce: reduce.c
	gcc -Wall $^ -o $@
	
test: clean all
	echo "inp0.txt" > desired.txt
	./fmr inp0.txt >> desired.txt
	echo "inp1.txt" >> desired.txt
	./fmr inp1.txt >> desired.txt
	echo "inp2.txt" >> desired.txt
	./fmr inp2.txt >> desired.txt
	echo "inp3.txt" >> desired.txt
	./fmr inp3.txt >> desired.txt
	echo "inp4.txt" >> desired.txt
	./fmr inp4.txt >> desired.txt
	
test_bonus: clean bonus test
	
clean:
	rm fmr filter map reduce desired.txt
