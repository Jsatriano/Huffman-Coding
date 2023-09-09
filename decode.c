#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

uint64_t bytes_read;
uint64_t bytes_written;

int main(int argc, char **argv) {
    int opt = 0;
    uint64_t extr_bytes = 0;
    bool got_i = false;
    bool got_o = false;
    bool got_v = false;
    bool verbose = false;
    char *input_file;
    char *output_file;
    int infile;
    int outfile;
    uint8_t in_buff;
    uint8_t out_buff;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            puts("PROGRAM SUMMARY");
            puts("A Huffman decoder.");
            puts("Uses the Huffman coding algorithm to decompress a file.");
            puts("");
            puts("EXAMPLE INPUT TO COMMAND LINE");
            puts("./encode [-h] [-v] [-i infile] [-o outfile]");
            puts("");
            puts("OPTIONS");
            puts("-h             Program usage and help.");
            puts("-v             print compression stats.");
            puts("-i infile      Input file to compress.");
            puts("-o outfile     Output of compressed data.");
            return 0;
        case 'i':
            got_i = true;
            input_file = optarg;
            break;
        case 'o':
            got_o = true;
            output_file = optarg;
            break;
        case 'v': got_v = true; break;
        default: break;
        }
    }
    if (got_i) {
        infile = open(input_file, O_RDONLY);
    } else {
        infile = read(STDIN_FILENO, &in_buff, sizeof(in_buff));
    }
    if (got_o) {
        outfile = open(output_file, O_RDWR | O_CREAT);
    } else {
        outfile = write(STDOUT_FILENO, &out_buff, sizeof(out_buff));
    }
    if (got_v) {
        verbose = true;
    }

    //check to see if header is the magic code
    Header h;
    extr_bytes += read_bytes(infile, (uint8_t *) &h, sizeof(Header));
    if (h.magic != MAGIC) {
        fprintf(
            stderr, "Error decoding file: file was not encoded. Please decode and encoded file\n");
        exit(0);
    }
    //set permissions of outfile
    fchmod(outfile, h.permissions);

    //read in dumped tree
    uint8_t *tree_dump = malloc(h.tree_size * sizeof(uint8_t));
    extr_bytes += read_bytes(infile, tree_dump, h.tree_size);
    
    //reconstruct the tree
    Node *root = rebuild_tree(h.tree_size, tree_dump);
    Node *curr = root;
    uint8_t bit;
    uint32_t written_sym = 0;
    while (written_sym != h.file_size) {
        read_bit(infile, &bit);
        if (bit == 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
        if (curr->left == NULL && curr->right == NULL) {
            write_bytes(outfile, &curr->symbol, 1);
            written_sym += 1;
            curr = root;
        }
    }
    
    //if the user enables verpose printing
    bytes_read = (bytes_read / 8) + extr_bytes;
    if (verbose) {
        float space_saved;
        space_saved = 100 * (1 - ((float) bytes_read / (float) bytes_written));
        fprintf(stderr, "Compressed file size %lu bytes\n", bytes_read);
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2f%%\n", space_saved);
    }
    close(infile);
    close(outfile);
    return 0;
}
