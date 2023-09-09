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
uint32_t dump_index = 0;

void postorder(Node *n, uint8_t treedump[]) {
    if (n != NULL) {
        postorder(n->left, treedump);
        postorder(n->right, treedump);

        if (n->left == NULL && n->right == NULL) { //if we are at a leaf node
            treedump[dump_index] = 'L'; //insrt L and the symbol of the node into array
            dump_index += 1;
            treedump[dump_index] = n->symbol;
            dump_index += 1;
        } else { //else we are at an interior node
            treedump[dump_index] = 'I'; //insert I into the array
            dump_index += 1;
        }
    }
    return;
}

int main(int argc, char **argv) {
    int opt = 0;
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
            puts("A Huffman encoder.");
            puts("Uses the Huffman coding algorithm to compress a file.");
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
        infile = read(STDIN_FILENO, &in_buff, sizeof(in_buff)); //probably wrong
    }
    if (got_o) {
        outfile = open(output_file, O_RDWR | O_CREAT);
    } else {
        outfile = write(STDOUT_FILENO, &out_buff, sizeof(out_buff)); //probably wrong
    }
    if (got_v) {
        verbose = true;
    }

    //set file permissions for header later on
    struct stat file_buf;
    fstat(infile, &file_buf);
    fchmod(outfile, file_buf.st_mode);

    //create a histogram
    static uint64_t num = 0;
    static uint64_t count[ALPHABET] = { 0 };
    int length;
    uint8_t buffer[BLOCK] = { 0 };
    while ((length = read_bytes(infile, buffer, BLOCK)) > 0) { //read through infile and increment occurences of each ASCII value
        num += length;
        for (int i = 0; i < length; i += 1) {
            count[buffer[i]] += 1;
        }
    }

    //increment element 0 and 255
    count[0] += 1;
    count[255] += 1;

    //go back to beginning of infile
    lseek(infile, 0, SEEK_SET);

    //build the Huffman tree
    Node *tree_root = build_tree(count);

    //create a code table
    Code table[ALPHABET] = { 0 };

    //build the code table by traversing the built Huffman tree
    build_codes(tree_root, table);

    //get the size of the created Huffman tree dump (used in header construction)
    int symbol = 0;
    for (int i = 0; i < ALPHABET; i += 1) {
        if (count[i] > 0) {
            symbol += 1;
        }
    }

    //construct a header
    Header h;
    h.magic = MAGIC;
    h.permissions = file_buf.st_mode;
    h.tree_size = (3 * symbol) - 1;
    h.file_size = file_buf.st_size;

    //write the header to outfile
    write_bytes(outfile, (uint8_t *) &h, sizeof(Header));

    //create postorder treedump array and index counter
    uint8_t *treedump = malloc(h.tree_size * sizeof(uint8_t));
    //uint32_t dump_index = 0;

    //perform post-order traversal of the tree, writing the treedump to the outfile
    postorder(tree_root, treedump);
    write_bytes(outfile, treedump, dump_index);

    //write out each constructed code to the outfile
    uint8_t read_buf[BLOCK] = { 0 };
    int len;
    while ((len = read_bytes(infile, read_buf, BLOCK)) > 0) {
        for (int i = 0; i < len; i += 1) {
            write_code(outfile, &table[read_buf[i]]);
        }
    }
    flush_codes(outfile); //write remaining codes (if there are any);
    free(treedump);

    //if verbose was enabled, print statistics
    bytes_read = h.file_size;
    if (verbose) {
        float space_saved;
        space_saved = 100 * (1 - ((float) bytes_written / (float) bytes_read));
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_read);
        fprintf(stderr, "Compressed file size %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2f%%\n", space_saved);
    }
    close(infile);
    close(outfile);
    return 0;
}
