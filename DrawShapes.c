#include <stdio.h>
#include <stdlib.h>

static int rows;
static int cols;

/* Initializes a 2D char array to all 0s */
void init_2D_array(char *arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i * cols + j] = 0;
        }
    }
}

/** Writes the input file's next line of coordinates in the array being pointed to. */
void write_next_coords(FILE *input, int *coords) {
    char ch;
    int dimensions = 2;

    for (int i = 0; i < dimensions; i++) {
        ch = fgetc(input);

        if (ch == 'E') {
            coords[i] = -1;
        } else if (ch < '0' || ch > '9') {
            coords[i] = -2;
        } else {
            coords[i] = ch - '0';
        }

        fgetc(input); // skipping this character
    }
}

/** 
 * Allocates the necessary memory in heap to draw the shape and then draws 
 * the shape's outline on a 2D array.
 */
char *outline_shape(FILE *input) {

    int largest_x = 0;
    int largest_y = 0;

    int coords[2];
    int *x = &coords[0];
    int *y = &coords[1];

    write_next_coords(input, coords);

    while (*x != -1 && *y != -1) {
        if (*x == -2 || *y == -2) {
            return NULL;
        } else {
            if (*x > largest_x) {
                largest_x = *x;
            }
            if (*y > largest_y) {
                largest_y = *y;
            }
        }
        write_next_coords(input, coords);
    }

    rows = largest_y + 1;
    cols = largest_x + 1;

    char *shape = malloc(rows * cols * sizeof(char));
    init_2D_array(shape, rows, cols);

    // Restart input stream
    rewind(input);
    write_next_coords(input, coords);

    int tempx = *x;
    int tempy = *y;
    shape[tempy * cols + tempx] = '*';

    int dx;
    int dy;
    while (*x != -1 && *y != -1) {
        dx = *x - tempx;
        dy = *y - tempy;
        if (dy == 0 && dx != 0) {
            for (int i = 1; i <= abs(dx); i++) {
                shape[tempy * cols + tempx + (i * dx/abs(dx))] = '*';
            }
        } else if (dx == 0 && dy != 0) {
            for (int i = 1; i <= abs(dy); i++) {
                shape[tempy * cols + tempx + (i * dy/abs(dy)) * cols] = '*';
            }
        } else if (dx != 0 && dy != 0) {
            for (int i = 1; i <= abs(dx); i++) {
                shape[tempy * cols + tempx + (i * dx/abs(dx)) + (i * dy/abs(dy)) * cols] = '*';
            }
        }

        tempx = *x;
        tempy = *y;
        write_next_coords(input, coords);
    }

    rewind(input);

    return shape;
}

/** Fills an outline of a shape with asterisks. */
void fill_shape(char *shape) {
    char ch = shape[0];
    char pair[2];

    for (int r = 0; r < rows; r++) {
        pair[0] = -1;
        pair[1] = -1;

        for (int c = 0; c < cols; c++) {
            ch = shape[r * cols + c];
            
            if (ch == '*') {
                if (pair[0] == -1) {
                    pair[0] = c + 1;
                } else {
                    pair[1] = c - 1;

                    for (int j = pair[0]; j <= pair[1]; j++) {
                        shape[r * cols + j] = '*';
                    }

                    break;
                }
            }
        }

        int col = cols - 1;
        while (shape[r * cols + col] != '*') {
            shape[r * cols + col] = 'E';
            col--;
        }
    }
}

/** Outputs the shape into a text file. */
void display_shape(char *shape, FILE *output) {
    char ch;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            ch = shape[(rows - 1 - r) * cols + c];
            
            if (ch == '*') {
                fputc(ch, output);
            } else if (ch != 'E') {
                fputc(' ', output);
            }
        }

        if (r != (rows - 1)) {
            fputc('\n', output);
        }
    }
}

/**
 * This program reads coordinates from an input file, creates a shape by
 * drawing lines, and then fills the area inside the shape.
 * All drawing is done with asterisks (*).
 * 
 * Use: ./lab4.out <input> <output>
 * input - Path of text file to be taken as input
 * output - Path of text file where output will be written
 */
int main(int argc, char **argv) {
    if (argc != 3) {
        // Error: Incorrect number of command line arguments
        fputs("error", fopen("./output.txt", "w"));
        return -1;
    }

    FILE *output = fopen(argv[2], "w");

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        // Error: Problem opening specified input file
        fputs("error", output);
        return -1;
    }

    char *shape = outline_shape(input);
    if (shape == NULL) {
        // Error: Problem in input format
        fputs("error", output);
        return -1;
    }

    fill_shape(shape);
    display_shape(shape, output);
    
    fclose(input);
    free(shape);

    return 0;
}