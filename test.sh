#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./3cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 '0;'
try 42 '42;'
try 21 '5+20-4;'
try 41 ' 12 + 34 - 5 ;'
try 17 ' 2 + 5 * 3 ;'
try 4 '2*5-3*2;'
try 6 '10-2*3+2;'
try 11 '3+20/5*2;'
try 5 '3+20/5-2;'
try 3 '(5+20)/5-2;'
try 40 '(10-2)*(3+2);'
try 5 '-10+15;'
try 42 '-10*2 + 62;'
try 1 '-(2+4*3) + 15;'
try 0 '-(2+4*3) + 15 == 2;'
try 1 '-10*2 + 62 == 42;'
try 1 '15 - (2+4*3) != 2;'
try 0 '-10*2 + 60 != 40;'
try 1 '15 - (2+4*3) < 2;'
try 0 '40 < -10*2 + 60;'
try 0 '15 - (2+3*3) > 8;'
try 1 '2*10 > -10*2 + 5*4;'
try 0 '15 - (2+3*3) <= 1;'
try 1 '40 <= -10*2 + 60;'
try 0 '15 - (2+3*3) >= 9;'
try 1 '2*10 >= 5*4;'
try 1 '(10 >= 10) == 1;'
try 1 '(10 >= 9) == 1;'
try 0 '(10 >= 11) == 1;'
try 42 '15 - (2+3*3) >= 9;2*10 >= 5*4;(10 >= 10) == 1;(10 >= 9) == 1;(10 >= 11) == 1;42;'
try 42 'a = 40;b = a + 2;b;'
try 1 'a = 5;a - 2 == 3;'
try 42 'a = 40;b = a + 2;return b;'
try 5 'a = 5;return a;a - 2 == 3;'
try 5 'a = b = 5;return b;return 3;'
try 42 'abc = 42;abc;'
try 84 'c_11 = 42;_lisp=42;return _lisp+c_11;0;'
try 0 'if (1) return 0;1;'
try 1 'if (0) return 0;1;'
try 1 'a = 1;if (a - 1) return 0;1;'
try 42 'Haskel=7;LISP = 42;if (LISP) return LISP;Haskell;'
try 0 'a = 1;if (a - 1) 1;else return 0;return 1;'
try 0 'if (1) 1;else return 1;return 0;'
try 5 'a=0;while (a<5) a = a + 1;a;'
try 6 'a=0;while (a<=5) a = a + 1;a;'
try 0 'a=5;while (a) a = a -1;a;'
try 0 'a=0;if (0) 1;a;'
try 0 'a=0;while (0) 1;a;'
try 5 'a=0;for (;;) if (a < 5) a = a + 1;else return a;0;'
try 6 'a=0;for (;;) if (a > 5) return a;else a = a+1;0;'
try 5 'for (a=0;;) if (a < 5) a = a + 1;else return a;0;'
try 10 'for (a=10;;) if (a < 5) a = a + 1;else return a;'
try 5 'for (a = 0;a < 5;) a = a + 1;a;'
try 10 'for (a = 10;a < 5;) a = a + 1;a;'

echo OK
