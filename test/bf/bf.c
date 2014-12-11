#include <stdio.h>
#include <stdlib.h>

#define TAPE_SIZE   30000
#define MAX_NESTING 100

typedef unsigned char uchar;

struct bf_state;

typedef uchar (*get_ch_func)(struct bf_state*);
typedef void (*put_ch_func)(struct bf_state*, uchar);

typedef struct bf_state {
    uchar* tape;
    get_ch_func    get_ch;
    put_ch_func    put_ch;
} bf_state_t;

#define bad_program(s) exit(fprintf(stderr, "bad program near %.16s: %s\n", program, s))

static void bf_interpret(const char* program, bf_state_t* state) {
    const char* loops[MAX_NESTING];

    int nloops = 0;
    int n;
    int nskip = 0;
    uchar* tape_begin = state->tape;
    uchar* ptr = state->tape;
    uchar* tape_end = state->tape + TAPE_SIZE - 1;
    const char* p = program;

    while (1) {
        switch (*p++) {
        case '>':
            for (n=1; *p == '>'; n++, p++);
            ptr += n;
            while (ptr > tape_end)
                ptr -= TAPE_SIZE;
            break;
        case '<':
            for (n=1; *p == '<'; n++, p++);
            ptr -= n;
            while (ptr < tape_begin)
                ptr += TAPE_SIZE;
            break;
        case '+':
            for (n=1; *p == '+'; n++, p++);
            *ptr += n;
            break;
        case '-':
            for (n=1; *p == '-'; n++, p++);
            *ptr -= n;
            break;
        case ',':
            *ptr = state->get_ch(state);
            break;
        case '.':
            state->put_ch(state, *ptr);
            break;
        case '[':
            if (nloops == MAX_NESTING)
                bad_program("Nesting too deep");
            if (*ptr) {
                loops[nloops++] = p;
                break;
            }
            nskip = 1;
            while (nskip > 0) {
                switch (*p++) {
                case '[': nskip++; break;
                case ']': nskip--; break;
                case '\0':
                    bad_program("end of file when there's open '['");
                    break;
                //ignore all other instruction.
                }
            }
            break;
        case ']':
            if (nloops <= 0)
                bad_program("] has no matched [");
            if (*ptr) {
                p = loops[nloops-1];
            } else {
                nloops --;
            }
            break;
        case '\0':
            if (nloops > 0)
                bad_program("end of file where there's open '['");
            return;
        //ignore all other instruction.
        }
    }
}

static uchar get_ch(bf_state_t *state) {
    return (unsigned char)getchar();
}

static void put_ch(bf_state_t *state, uchar c) {
    putchar((int)c);
}

static void bf_run(const char* program) {
    bf_state_t state;
    uchar tape[TAPE_SIZE];
    state.tape = tape;
    state.get_ch = get_ch;
    state.put_ch = put_ch;
    bf_interpret(program, &state);
}

int main(int argc, char* argv[]) {
    long sz;
    char *program;
    FILE* fp;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s INFILE.bf\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Can't open file '%s'\n", argv[1]);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    program = (char*)malloc(sz + 1);
    program[fread(program, 1, sz, fp)] = 0;
    fclose(fp);
    bf_run(program);
    return 0;
}
