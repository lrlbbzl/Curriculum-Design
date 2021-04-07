# 建立棋盘
import random
import os

filename = os.listdir(r'D:\课设\最后一把\Debug')
puzzle = []


def search():
    for s in filename:
        if s[-1] == 's' and s[-2] == 'e':
            puzzle.append("D:\课设\最后一把\Debug\\"+s[:-4]+".puz")
    # print(puzzle)


def check(matrix, i, j, number):
    if i == 0 and matrix[i+1][j] == number and matrix[i+2][j] == number:
        return False
    elif i == 5 and matrix[i-1][j] == number and matrix[i-2][j] == number:
        return False
    elif 1 <= i <= 4 and matrix[i-1][j] == number and matrix[i+1][j] == number:
        return False

    if j == 0 and matrix[i][j+1] == number and matrix[i][j+2] == number:
        return False
    elif j == 5 and matrix[i][j-1] == number and matrix[i][j-2] == number:
        return False
    elif 1 <= j <= 4 and matrix[i][j-1] == number and matrix[i][j+1] == number:
        return False

    num_0, num_1 = 0, 0
    for col in range(len(matrix[i])):
        if matrix[i][col] == 1:
            num_1 += 1
        elif matrix[i][col] == 0:
            num_0 += 1
    if num_0 > 3 or num_1 > 3:
        return False

    num_0, num_1 = 0, 0
    for row in range(len(matrix)):
        if matrix[row][j] == 1:
            num_1 += 1
        elif matrix[row][j] == 0:
            num_0 += 1
    if num_0 > 3 or num_1 > 3:
        return False

    return True


def Generate(n):
    length = len(puzzle)
    n = n % length
    str = puzzle[n]
    str1 = str[:-4]+".res"
    matrix = [[] for i in range(6)]
    ans_matrix = [[] for i in range(6)]
    row = 0
    fp = open(str,'r')
    fp1 = open(str1,'r')
    blank_ij = []
    string = fp.readline()
    while string:
        column = 0
        for ch in string:
            if ch == '.':
                matrix[row].append(-1)
                blank_ij.append((row,column))

            elif ch == '1':
                matrix[row].append(1)

            elif ch == '0':
                matrix[row].append(0)
            column += 1

        string = fp.readline()
        row += 1
    # print(blank_ij)
    row = 0
    string = fp1.readline()
    while string:
        for ch in string:
            if ch == '0':
                ans_matrix[row].append(0)
            elif ch == '1':
                ans_matrix[row].append(1)
        string = fp1.readline()
        row += 1

    return matrix, ans_matrix, blank_ij
