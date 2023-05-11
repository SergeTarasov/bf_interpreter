Dead simple bf interpreter. 

Contains kind of bf virtual machine, but its modified so bf would execute faster.
VM "word" consists of two parts: command number and parameter.
3, 10 - add 3 to current cell
1, 9  - move data pointer 9 cells forward
7, 1950 - if current cell value is zero, jump to 1950
8, 1947 - if current cell value is not zero, jump to 1947

mandelbrot.b execution time is about 12 seconds.

#### mandelbrot.b benchmark:
1. w/o compilation **72 sec**
2. with loop opt. **33 sec**
3. with stacked operands opt. **12 sec**
