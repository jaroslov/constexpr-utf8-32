#ifndef CXUTF8232_HPP
#define CXUTF8232_HPP

namespace CXUTF8232
{

template <int K>
constexpr unsigned long utf32chk(const char (&msg)[K], int at=0)
{
    return  (at < K)
        && (
                0
            ||  ((msg[at] & 0x80) == 0x00)
            ||  ( ((msg[at] & 0xE0) == 0xC0) && (K >= (2 + at) && ((msg[at+1] & 0xC0) == 0x80)) )
            ||  ( ((msg[at] & 0xF0) == 0xE0) && (K >= (3 + at) && ((msg[at+1] & 0xC0) == 0x80) && ((msg[at+2] & 0xC0) == 0x80)) )
            ||  ( ((msg[at] & 0xF8) == 0xF0) && (K >= (4 + at) && ((msg[at+1] & 0xC0) == 0x80) && ((msg[at+2] & 0xC0) == 0x80)  && ((msg[at+3] & 0xC0) == 0x80)) )
            ||  0
        );
}

constexpr int utf32_is1(const char C)
{
    return (C & 0x80) == 0x00;
}

constexpr int utf32_is2(const char C)
{
    return (C & 0xE0) == 0xC0;
}

constexpr int utf32_is3(const char C)
{
    return (C & 0xF0) == 0xE0;
}

constexpr int utf32_is4(const char C)
{
    return (C & 0xF8) == 0xF0;
}

constexpr signed long utf32_gethead(const char C)
{
    return utf32_is1(C)
            ? C
            : (utf32_is2(C)
                ? ((unsigned long)(C & 0x1F) << 6)
                : (utf32_is3(C)
                    ? ((unsigned long)(C & 0x0F) << 12)
                    : ((unsigned long)(C & 0x07) << 18)))
            ;
}

template <int K>
constexpr signed long utf32_getlimb(const char (&msg)[K], int limb, int in, int at=0)
{
    return (msg[1 + limb + at] & 0x3F) << (6 * (in - limb));
}

template <int K>
constexpr signed long utf32_getbody(const char (&msg)[K], int at=0)
{
    return utf32_is1(msg[at])
            ? 0
            : (utf32_is2(msg[at])
                ? utf32_getlimb(msg, 0, 0, at)
                : (utf32_is3(msg[at])
                    ? (utf32_getlimb(msg, 0, 1, at) | utf32_getlimb(msg, 1, 1, at))
                    : (utf32_getlimb(msg, 0, 2, at) | utf32_getlimb(msg, 1, 2, at) | utf32_getlimb(msg, 2, 2, at))))
            ;
}

template <int K>
constexpr signed long utf32(const char (&msg)[K], int at=0)
{
    // Converts the first sequence of utf8 chars into
    // a UTF32 at offset 'at'.
    return utf32chk(msg, at)
            ? (utf32_gethead(msg[at]) | utf32_getbody(msg, at))
            : -1
            ;
}

struct utf8a { char str[5]; };

constexpr utf8a utf8(signed long u32)
{
    return (u32 < 0x80)
        ? utf8a { .str = { (char)u32 } }
        : ((u32 < (1 << 12))
            ? utf8a{ .str = { (char)(0xC0 | (u32 >> 6)), (char)(0x80 | ((u32 >> 0) & 0x3F)) }, }
            : ((u32 < (1 << 17))
                ? utf8a{ .str = { (char)(0xE0 | (u32 >> 12)), (char)(0x80 | ((u32 >>  6) & 0x3F)), (char)(0x80 | ((u32 >> 0) & 0x3F)) } }
                : utf8a{ .str = { (char)(0xF0 | (u32 >> 18)), (char)(0x80 | ((u32 >> 12) & 0x3F)), (char)(0x80 | ((u32 >> 6) & 0x3F)), (char)(0x80 | ((u32 >> 0) & 0x3F)) } })
        )
        ;
}

}//CXUTF8232

#endif//CXUTF8232_HPP
