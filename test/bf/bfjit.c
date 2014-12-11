/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.3.0, DynASM a64 version 1.3.0
** DO NOT EDIT! The original file is in "bf_a64.dasc".
*/

#include <stdio.h>
#include <stdlib.h>
#define DASM_CHECKS 1
#include "../../src/dasm_proto.h"
#include "../../src/dasm_a64.h"

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

typedef void (*bfprogram)(bf_state_t* state);

#define bad_program(s) exit(fprintf(stderr, "bad program near %.16s: %s\n", program, s))

void dumpcode(void* buf, size_t sz) {
    int* b = (int*)buf;
    int* end = (int*)((char*)buf + sz);

    for (;b<end; b++)
        printf("%08x: %08x\n", b, *b);
}

#include <sys/mman.h>
static void* link_and_encode(dasm_State**d) {
    size_t sz;
    void* buf;
    dasm_link(d, &sz);
    buf = mmap(0, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    dasm_encode(d, buf);
    //dumpcode(buf, sz);
    mprotect(buf, sz, PROT_READ|PROT_EXEC);
    return buf;
}

static bfprogram bf_compile(const char* program) {
    unsigned int loops[MAX_NESTING];
    int nloops = 0;
    int n;
//    int nskip = 0;
//    uchar* tape_begin = state->tape;
//   uchar* ptr = state->tape;
//    uchar* tape_end = state->tape + TAPE_SIZE - 1;

    dasm_State* d;
    unsigned int npc = 8;
    unsigned int nextpc = 0;

    //|.arch a64
#if DASM_VERSION != 10300
#error "Version mismatch between DynASM and included encoding engine"
#endif

    //|.section code
#define DASM_SECTION_CODE	0
#define DASM_MAXSECTION		1

    dasm_init(&d, DASM_MAXSECTION);

    //|.globals bf_
enum {
  bf_main,
  bf__MAX
};
    void* labels[bf__MAX];
    dasm_setupglobal(&d, labels, bf__MAX);

    //|.actionlist bf_actions
static const unsigned int bf_actions[87] = {
0x00010000,
0x00060014,
0xd10103ff,
0xa90053f3,
0xa9015bf5,
0xa90263f7,
0xf9001bfe,
0xaa0003f4,
0xf9400293,
0x00090d8a,
0xaa1303f5,
0x90000017,
0x000f0001,
0x8b170276,
0x00000000,
0x90000017,
0x000f0001,
0x8b170273,
0xeb16027f,
0x54000009,
0x00053001,
0x90000017,
0x000f0001,
0xcb170273,
0x0006000b,
0x00000000,
0x90000017,
0x000f0001,
0xcb170273,
0xeb15027f,
0x54000002,
0x00053001,
0x90000017,
0x000f0001,
0x8b170273,
0x0006000b,
0x00000000,
0x39400277,
0x00020000,
0x00000018,
0x000f0000,
0x0b1802f7,
0x39000277,
0x00000000,
0x39400277,
0x00020000,
0x00000018,
0x000f0000,
0x4b1802f7,
0x39000277,
0x00000000,
0xaa1403e0,
0xf9400297,
0x00090d8a,
0xd63f02e0,
0x39000260,
0x00000000,
0x39400261,
0xaa1403e0,
0xf9400297,
0x00090d8a,
0xd63f02e0,
0x00000000,
0x00020000,
0x00000017,
0x000f0000,
0x39000277,
0x00000000,
0x39400277,
0x710002ff,
0x54000000,
0x00073000,
0x00080000,
0x00000000,
0x39400277,
0x710002ff,
0x54000001,
0x00073000,
0x00080000,
0x00000000,
0xa94053f3,
0xa9415bf5,
0xa94263f7,
0xf9401bfe,
0x910103ff,
0xd65f03c0,
0x00000000
};

    dasm_setup(&d, bf_actions);

    dasm_growpc(&d, npc);

    //|.define aPtr, x19
    //|.define aState, x20
    //|.define aTapeBegin, x21
    //|.define aTapeEnd, x22
    //|.define TMP, x23
    //|.define TMP1, x24
    //|.define TMPw, w23
    //|.define TMP1w, w24
    //|.define cArg1w, w0
    //|.define cArg2w, w1
    //|.define cRetw, w0
    //|.define cArg1, x0
    //|.define cArg2, x1
    //|.define cRet, x0
    //|.macro prologue
        //| sub sp, sp, #0x40
        //| stp aPtr, aState, [sp]
        //| stp aTapeBegin, aTapeEnd, [sp, #0x10]
        //| stp TMP, TMP1, [sp, #0x20]
        //| str lr, [sp, #0x30]
        //| mov aState, cArg1
    //|.endmacro
    //|.macro epilogue
        //| ldp aPtr, aState, [sp]
        //| ldp aTapeBegin, aTapeEnd, [sp, #0x10]
        //| ldp TMP, TMP1, [sp, #0x20]
        //| ldr lr, [sp, #0x30]
        //| add sp, sp, #0x40
        //| ret
    //|.endmacro

    const char* p = program;

    //|.type bfstate, bf_state_t, aState
#define Dt1(_V) (int)(ptrdiff_t)&(((bf_state_t *)0)_V)

    dasm_State** Dst = &d;
    //|.code
    dasm_put(Dst, 0);
    //|->main:
    //| prologue
    //| ldr aPtr, bfstate->tape
    //| mov aTapeBegin, aPtr
    //| mov TMP, #TAPE_SIZE - 1
    //| add aTapeEnd, aPtr, TMP
    dasm_put(Dst, 1, (long)(Dt1(->tape)), (long)(TAPE_SIZE - 1));
    while (1) {
        switch (*p++) {
        case '>':
            for (n=1; *p == '>'; n++, p++);
            // ptr += n;
            // while (ptr > tape_end)
            //     ptr -= TAPE_SIZE;
            //| mov TMP, #(n%TAPE_SIZE)
            //| add aPtr, aPtr, TMP
            //| cmp aPtr, aTapeEnd
            //| b.ls >1
            //| mov TMP, #TAPE_SIZE
            //| sub aPtr, aPtr, TMP
            //|1:
            dasm_put(Dst, 15, (long)((n%TAPE_SIZE)), (long)(TAPE_SIZE));
            break;
        case '<':
            for (n=1; *p == '<'; n++, p++);
            //ptr -= n;
            //while (ptr < tape_begin)
            //    ptr += TAPE_SIZE;
            //| mov TMP, #(n%TAPE_SIZE)
            //| sub aPtr, aPtr, TMP
            //| cmp aPtr, aTapeBegin
            //| b.hs >1
            //| mov TMP, #TAPE_SIZE
            //| add aPtr, aPtr, TMP
            //|1:
            dasm_put(Dst, 26, (long)((n%TAPE_SIZE)), (long)(TAPE_SIZE));
            break;
        case '+':
            for (n=1; *p == '+'; n++, p++);
            //*ptr += n;
            //| ldrb TMPw, [aPtr]
            //| mov TMP1w, #n
            //| add TMPw, TMPw, TMP1w
            //| strb TMPw, [aPtr]
            dasm_put(Dst, 37, (long)(n));
            break;
        case '-':
            for (n=1; *p == '-'; n++, p++);
            //*ptr -= n;
            //| ldrb TMPw, [aPtr]
            //| mov TMP1w, #n
            //| sub TMPw, TMPw, TMP1w
            //| strb TMPw, [aPtr]
            dasm_put(Dst, 44, (long)(n));
            break;
        case ',':
            //*ptr = state->get_ch(state);
            //| mov cArg1, aState
            //| ldr TMP, bfstate->get_ch
            //| blr TMP
            //| strb cRetw, [aPtr]
            dasm_put(Dst, 51, (long)(Dt1(->get_ch)));
            break;
        case '.':
            //state->put_ch(state, *ptr);
            //| ldrb cArg2w, [aPtr]
            //| mov cArg1, aState
            //| ldr TMP, bfstate->put_ch
            //| blr TMP
            dasm_put(Dst, 57, (long)(Dt1(->put_ch)));
            break;
        case '[':
            if (nloops == MAX_NESTING)
                bad_program("Nesting too deep");
            //if (*ptr) {
            //    loops[nloops++] = p;
            //    break;
            //}
            //nskip = 1;
            //while (nskip > 0) {
            //    switch (*p++) {
            //    case '[': nskip++; break;
            //    case ']': nskip--; break;
            //    case '\0':
            //        bad_program("end of file when there's open '['");
            //        break;
            //    //ignore all other instruction.
            //    }
            //}

            if (p[0] == '-' && p[1] == ']') {
                p += 2;
                //| mov TMPw, #0
                //| strb TMPw, [aPtr]
                dasm_put(Dst, 63, (long)(0));
            } else {
                if (nextpc == npc) {
                    npc *= 2;
                    dasm_growpc(&d, npc);
                }
                //| ldrb TMPw, [aPtr]
                //| cmp TMPw, #0
                //| b.eq =>nextpc + 1
                //|=>nextpc:
                dasm_put(Dst, 68, (long)(nextpc + 1), (long)(nextpc));
                loops[nloops++] = nextpc;
                nextpc += 2;
            }
            break;
        case ']':
            if (nloops <= 0)
                bad_program("] has no matched [");
            //if (*ptr) {
            //    p = loops[nloops-1];
            //} else {
            //    nloops --;
            //}
            --nloops;
            //| ldrb TMPw, [aPtr]
            //| cmp TMPw, #0
            //| b.ne =>loops[nloops]
            //|=>loops[nloops]+1:
            dasm_put(Dst, 74, (long)(loops[nloops]), (long)(loops[nloops]+1));
            break;
        case '\0':
            if (nloops > 0)
                bad_program("end of file where there's open '['");
            // return;
            //| epilogue
            dasm_put(Dst, 80);
            link_and_encode(&d);
            dasm_free(&d);
            return (bfprogram)labels[bf_main];
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
    bfprogram bfp;
    state.tape = tape;
    state.get_ch = get_ch;
    state.put_ch = put_ch;
    bfp = bf_compile(program);
    bfp(&state);
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
