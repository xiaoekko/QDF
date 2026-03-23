import re
with open('c:/Users/18194/Desktop/QDF-3-19/QDF-HAL/User/keyapp.c', 'r', encoding='gbk', errors='ignore') as f:
    lines = f.readlines()
for i, line in enumerate(lines):
    if 'MenuID==3' in line.replace(' ', '').replace('\t', ''):
        start = max(0, i - 2)
        end = min(len(lines), i + 20)
        print(f'Match at line {i+1}:')
        for j in range(start, end):
            print(f'{j+1}: {lines[j].rstrip()}')
        print('---')
