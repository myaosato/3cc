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


echo OK
