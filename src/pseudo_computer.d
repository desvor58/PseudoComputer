import std.stdio;
import std.file;
import std.format;
import types;

static const sector_size = 512;
static const segment_size = 65_536;

static Reg ax;
static Reg bx;
static Reg cx;
static Reg dx;

static ushort CS = 0;
static ushort DS = 0;
static ushort SS = 0;

static ushort IP = 0;
static ushort RSP = 0;

byte[] mem;

int main(string[] args)
{
    ubyte[] disk = cast(ubyte[])read(args[1]);

    ax = new Reg();
    bx = new Reg();
    cx = new Reg();
    dx = new Reg();

    mem.length = 4_294_967_296;

    for (int i = 0; i < sector_size; i++) {
        mem[i] = disk[i];
    }

    execute();

    writeln(format("ax: %d h: %d l: %d", ax.x, ax.h, ax.l));
    writeln(format("bx: %d h: %d l: %d", bx.x, bx.h, bx.l));
    writeln(format("cx: %d h: %d l: %d", cx.x, cx.h, cx.l));
    writeln(format("dx: %d h: %d l: %d", dx.x, dx.h, dx.l));
    writeln();
    write("CS: ");  write(CS); write(", DS: "); write(DS); write(", SS: "); writeln(SS);
    write("RSP: "); write(RSP); write(", IP: "); write(IP);

    return 0;
}

void execute()
{
    SS  = cast(short)(CS + 1);
    RSP = segment_size - 1;

    ubyte comm;
    ubyte arg1;
    ubyte arg2;
    ubyte arg3;

    for (; IP < segment_size - 4; IP += 4) {
        comm = mem[CS + IP];
        arg1 = mem[CS + IP + 1];
        arg2 = mem[CS + IP + 2];
        arg3 = mem[CS + IP + 3];

        execute_comm(comm, arg1, arg2, arg3);
    }
}

void execute_comm(ubyte comm, ubyte arg1, ubyte arg2, ubyte arg3)
{
    switch (comm) {
        case 0x00:                  /* MOV */
            set(arg1, get(arg2));
            break;
        case 0x80:                  /* СMOV */
            set(arg1, cast(short)((arg2 << 8) + arg3));
            break;
        case 0x01:                  /* ADD */
            set(arg1, cast(short)(get(arg1) + get(arg2)));
            break;
        case 0x81:                  /* CADD */
            set(arg1, cast(short)(get(arg1) + cast(short)(arg2 + arg3)));
            break;
        case 0x02:                  /* SUB */
            set(arg1, cast(short)(get(arg1) - get(arg2)));
            break;
        case 0x82:                  /* CSUB */
            set(arg1, cast(short)(get(arg1) - cast(short)(arg2 + arg3)));
            break;
        case 0x03:                  /* AND */
            set(arg1, cast(short)(get(arg1) && get(arg2)));
            break;
        case 0x83:                  /* CAND */
            set(arg1, cast(short)(get(arg1) && cast(short)(arg2 + arg3)));
            break;
        case 0x04:                  /* OR */
            set(arg1, cast(short)(get(arg1) || get(arg2)));
            break;
        case 0x84:                  /* COR */
            set(arg1, cast(short)(get(arg1) || cast(short)(arg2 + arg3)));
            break;
        case 0x05:                  /* XOR */
            set(arg1, cast(short)(get(arg1) ^^ get(arg2)));
            break;
        case 0x85:                  /* CXOR */
            set(arg1, cast(short)(get(arg1) ^^ cast(short)(arg2 + arg3)));
            break;
        case 0x06:                  /* NOT */
            set(arg1, !get(arg2));
            break;
        case 0x86:                  /* CNOT */
            set(arg1, !(arg2 + arg3));
            break;
        case 0x07:                  /* SHL */
            set(arg1, cast(short)(get(arg1) << get(arg2)));
            break;
        case 0x87:                  /* CSHL */
            set(arg1, cast(short)(get(arg1) << cast(short)(arg2 + arg3)));
            break;
        case 0x08:                  /* SHR */
            set(arg1, cast(short)(get(arg1) >> get(arg2)));
            break;
        case 0x88:                  /* CSHR */
            set(arg1, cast(short)(get(arg1) >> cast(short)(arg2 + arg3)));
            break;

        case 0x09:                 /* JMP */
            IP = cast(ushort)((arg1 << 8) + arg2 - 4);
            break;
        case 0x0A:                 /* JEZ */
            if (get(arg1) == 0) {
                IP = cast(ushort)((arg2 << 8) + arg3 - 4);
            }
            break;
        case 0x0B:                 /* JGZ */
            if (get(arg1) > 0) {
                IP = cast(ushort)((arg2 << 8) + arg3 - 4);
            }
            break;
        case 0x0C:                 /* JLZ */
            if (get(arg1) < 0) {
                IP = cast(ushort)((arg2 << 8) + arg3 - 4);
            }
            break;

        case 0x0D:                 /* PUSH */
            writeln("PUSH");
            mem[SS + RSP] = cast(byte)get(arg1);
            RSP--;
            mem[SS + RSP] = cast(byte)(get(arg1) >> 8);
            RSP--;
            break;
        case 0x8D:                 /* CPUSH */
            mem[SS + RSP] = arg2;
            RSP--;
            mem[SS + RSP] = arg1;
            RSP--;
            break;
        case 0x4D:  /* 01001101 */ /* PUSHB */
            mem[SS + RSP] = cast(byte)get(arg1);
            RSP--;
            break;
        case 0xCD:  /* 11001101 */ /* CPUSHB */
            mem[SS + RSP] = arg1;
            RSP--;
            break;
        case 0x0E:                 /* POP */
            RSP++;
            set(arg1, cast(short)((mem[SS + RSP] << 8) + mem[SS + ++RSP]));
            break;
        case 0x4E:  /* 01001110 */ /* POPB */
            RSP++;
            set(arg1, cast(short)(mem[SS + RSP]));
            break;
        
        default: break;
    }
}

void set(int num, short val)
{
    switch (num) {
        case 0x00:
            ax.set(0, val);
            return;
        case 0x01:
            ax.set(1, cast(byte)val);
            return;
        case 0x02:
            ax.set(2, cast(byte)val);
            return;
            
        case 0x10:
            bx.set(0, val);
            return;
        case 0x11:
            bx.set(1, cast(byte)val);
            return;
        case 0x12:
            bx.set(2, cast(byte)val);
            return;
            
        case 0x20:
            cx.set(0, val);
            return;
        case 0x21:
            cx.set(1, cast(byte)val);
            return;
        case 0x22:
            cx.set(2, cast(byte)val);
            return;
            
        case 0x30:
            dx.set(0, val);
            return;
        case 0x31:
            dx.set(1, cast(byte)val);
            return;
        case 0x32:
            dx.set(2, cast(byte)val);
            return;

        case 0x40:
            CS = val;
            return;
        case 0x50:
            DS = val;
            return;
        case 0x60:
            SS = val;
            return;

        case 0x70:
            IP = val;
            return;
        case 0x80:
            RSP = val;
            return;
        
        default: break;
    }
}

short get(int num)
{
    switch (num) {
        case 0x00:
            return ax.x;
        case 0x01:
            return cast(short)ax.l;
        case 0x02:
            return cast(short)ax.h;
            
        case 0x10:
            return bx.x;
        case 0x11:
            return cast(short)bx.l;
        case 0x12:
            return cast(short)bx.h;
            
        case 0x20:
            return cx.x;
        case 0x21:
            return cast(short)cx.l;
        case 0x22:
            return cast(short)cx.h;
            
        case 0x30:
            return dx.x;
        case 0x31:
            return cast(short)dx.l;
        case 0x32:
            return cast(short)dx.h;

        case 0x40:
            return CS;
        case 0x50:
            return DS;
        case 0x60:
            return SS;
        
        case 0x70:
            return IP;
        case 0x80:
            return RSP;
        
        default: break;
    }
    return 0;
}