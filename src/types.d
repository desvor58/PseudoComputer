module types;

class Reg
{
    byte  h;
    byte  l;
    short x;

    this(short cx = 0, byte ch = 0, byte cl = 0)
    {
        x = cx;
        h = ch;
        l = cl;
    }

    void set(int num, short val)
    {
        switch (num)
        {
            case 0:
                x = val;
                l = cast(byte)val;
                h = cast(byte)(val >> 8);
                return;
            case 1:
                l = cast(byte)val;
                x = cast(short)((h >> 8) + l);
                return;
            case 2:
                h = cast(byte)val;
                x = cast(short)((h << 8) + l);
                return;
        
            default: break;
        }
    }
}

struct Regs
{
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
}