import sys
import random

T_MAX = int(sys.argv[1])
N_MAX = int(sys.argv[2])
S_MAX = int(sys.argv[3])

chars = "abc"
def gen_str():
    slen = random.randint(1, S_MAX)
    s = ""
    for _ in range(slen):
        s += random.choice(chars)
    return s

t = random.randint(1, T_MAX)
print(t)

for _ in range(t):
    n = random.randint(1, N_MAX)
    print(n)

    for _ in range(n):
        s = gen_str()
        print(s)
