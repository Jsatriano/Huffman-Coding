DESCRIPTION

The encoder and decoder programs contained in this folder use Huffman coding to compress and decompress files.

HOW TO RUN ENCODE
	
	1) 1) In terminal. navigate to where the files were downloaded.
	
	2) Once there, type "make", "make all" or "make encode" on the command line
	to create the encode executable.
	
	3) Type "./encode" followed by any combination of the options listed below.
	
	OPTIONS
	
	-h   |   -v   |   -i   |   -o
	
	> -h will print out a help message that describes the
	options listed above in detail.
	
	> -v will print compression statistics
	
	> -i {input} will decide what file will be taken as input into the program
	
	> -o {input} will decide what file the compressed output will be written on
	
HOW TO RUN DECODE

	1) Follow steps 1, 2, and 3 from the encode instructions. NOTE: instead of
	typing "make encode", you would type "make decode".
	
	OPTIONS
	
	-h   |   -v   |   -i   |   -o
	
	> -h will print out a help message that describes the
	options listed above in detail.
	
	> -v will print decompression statistics
	
	> -i {input} will decide what file will be taken as input into the program
	
	> -o {input} will decide what file the decompressed output will be written on
	
	
