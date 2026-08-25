# Cemgi
Another fucking chess engine. Written in C this time because i wanna learn C.

### Notes
    -add support for cpus with and without BMI
    -add tests and benchmarks for movegen
    
### Features
    -magic bitboards
    -bitboard representation

### Building
i have no idea. use makefile?

#### Useful commands 
    -checks cpu for SIMD and BMI related flags: lscpu | grep -oE 'sse[0-9_]*|ssse3|avx[0-9_]*|fma|f16c|bmi[0-9_]*' | sort -u

