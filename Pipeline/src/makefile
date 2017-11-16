ALL: 
	@gcc -o pager pager.c
	@gcc -o wordsearch wordsearch.c
	@gcc -o wordgen wordgen.c
	gcc -o launcher launcher.c

.PHONY:debug,clean
debug:
	@gcc -o pagerdebug  pager.c -g
	@gcc -o wordsearchdebug  wordsearch.c -g
	@gcc -o wordgendebug  wordgen.c -g
	@gcc -o launcherdebug launcher.c -g


clean:
	rm -f *.dSYM
	rm wordgen
	rm wordsearch
	rm pager
	rm launcher
