import sys
import random

T_MAX = int(sys.argv[1])
N_MAX = int(sys.argv[2])
KEY_MAX = int(sys.argv[3])
S_MAX = int(sys.argv[4])

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
        type = random.randint(0,1)
        key = random.randint(1, KEY_MAX)
        if type == 0:
            s = gen_str()
            print(type, key, s)
        else:
            print(type, key)
