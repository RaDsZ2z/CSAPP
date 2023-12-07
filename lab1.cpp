/*
  author      :rads
  finish time :2023.12.3
*/

/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  int v1 = x & y;
  int v2 = (~x) & (~y);
  return ((~v1) & (~v2));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int t = x + 1;
  int y = ~t;
  int res1 = !(y ^ x);//加1取反后和自己相等
  int res2 = !!t;//且加1之后的值不能是0
  return res1 & res2;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int t = 0xAA;
  t |= t << 8;
  t |= t << 16;
  x &= t;
  t ^= x;
  return !t;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int v1 = 0x30;
  int v2 = 0x39;

  // int neg = !!(x >> 31);
  int res1 = x + (~v1 + 1);//x - 0x30
  res1 = !(res1 >> 31);//符号位为0则为1 符号位为1则为0
  int res2 = v2 + (~x + 1);//0x39 - x
  res2 = !(res2 >> 31);
  // cout<<res1<<' '<<res2<<'\n';
  return res1 & res2;//需要两个res都为1 即两次相减符号位都为0
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  x=!!(x);
  x |= x << 1;
  x |= x << 2;
  x |= x << 4;
  x |= x << 8;
  x |= x << 16;
  return (x&y) | ((~x)&z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int Tmin = 1 << 31;
  // int Tmax = ~ Tmin;

  int sign1 = !!(x & Tmin);//符号位
  int sign2 = !!(y & Tmin);//符号位
  int sign3 = sign1 ^ sign2;//符号不同
  // int res1 = sign3 & sign2;//+ -
  int res2 = sign3 & sign1;//- +
  // cout<<sign1<<' '<<sign2<<' '<<sign3<<'\n';
  x = ~x +1;
  x += y;
  int res3 = !!(x & Tmin);//0: y-x>=0 1:y-x<0 即 0: x<=y 1:x>y
  res3 =! res3;//0: x>y 返回0  1:x<=y 返回1
  //以上两行可以简化为!(x & Tmin) 但是为了可读性还是保留现有代码
  return (sign3 & res2) | ((!sign3) & res3);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  x |= x>>16;
  x |= x>>8;
  x |= x>>4;
  x |= x>>2;
  x |= x>>1;
  x &= 1;
  x ^= 1;
  return x;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  //这个实在想不到 参考了一下网上的做法
  int mask = x>>31;//负就全1 否则全0

  
  x = ((~mask) & x) | (mask & (~x));

  int bit_16 = (!!(x >> 16))<<4;//高16位有没有1
  x >>= bit_16;//有则右移16位 否则不动 接下来处理位于低16位的值

  int bit_8 = (!!(x >> 8))<<3;//高8位有没有1
  x >>= bit_8;//有则右移8位 否则不动 接下来处理位于低8位的值

  int bit_4 = (!!(x >> 4))<<2;
  x >>= bit_4;
  int bit_2 = (!!(x >> 2))<<1;
  x >>= bit_2;
  int bit_1 = (!!(x >> 1))<<0;
  x >>= bit_1;
  int bit_0 = x;
  return bit_16 + bit_8 + bit_4 + bit_2 + bit_1 + bit_0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  //1 8 23
  //31  23～30 0～22
  //float point argument
  unsigned sign = uf >> 31;
  unsigned exp = (uf << 1) >> 24;
  unsigned frac = (uf << 9) >> 9;
  // int res;
  if(exp == 0xFF){
    // res = uf;
  }else if(exp == 0){
    //把尾数左移一位
    //若左移前尾数最高位为1 则把阶码+1
    exp = frac >> 22;
    frac = (frac << 10) >> 10;
    frac <<= 1;
    // res = (sign << 31) + (exp << 23) + frac;
  }else{
    exp++;
    //如果exp = 0xFE(11111110b) 应该返回inf / Nan
    //否则exp++
    //但是前一种情况直接exp++似乎就能达到效果了
    // res = (sign << 31) + (exp << 23) + frac;
  }
  return (sign << 31) + (exp << 23) + frac;
  // return res;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  // show(uf);
  int sign = uf >> 31;
  int exp = (uf << 1) >> 24;
  int frac = (uf << 9) >> 9;
  //1 8 23
  // -127
  int res;
  if(exp - 127 > 30){
    res = 0x80000000u;
  }
  // else if(exp == 0xFF){
  //  res = 0x80000000u;
  // }
  else if(exp - 127 < 0){
    res = 0;
  }
  // else if(exp == 0){
  //  res = 0;
  // }
  else{
    exp -= 127;
    if(exp >= 23){
      res = frac << (exp -23);
      res |= (1 << exp);
    }else{
      res = frac >> (23 - exp);
      res |= (1 << exp);
    }
  }
  if(sign)res = -res;
  return res;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  // [-126,127]
  unsigned res;
  if(x < -126){
    res = 0;
  }
  else if(x > 127) {
    // res = 0b01111111100000000000000000000000;
    res = 0x7f800000;
  }
  else{
    unsigned  exp = x + 127;
    res = exp << 23;
  }
  return res;
}
