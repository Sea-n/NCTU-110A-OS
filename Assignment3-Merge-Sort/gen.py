from random import randint

N = int(150000)
with open('input.txt', 'w') as f:
    f.write(f'{N}\n')
    for i in range(N):
        f.write(f'{randint(42, 2147483600)} ')
