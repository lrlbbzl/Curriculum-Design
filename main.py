import sys
import pygame
import tkinter
import win32api, win32con
import tkinter.messagebox as tm
from pygame.color import THECOLORS as COLORS
from build import Generate, check, search
from button import Button


def draw_background():
    # white background
    # screen.fill(COLORS['white'])
    screen.blit(background,[0,0])
    # draw game board
    pygame.draw.rect(screen, COLORS['black'], (0, 0, 300, 600), 5)
    pygame.draw.rect(screen, COLORS['black'], (300, 0, 300, 600), 5)

    pygame.draw.rect(screen, COLORS['black'], (0, 0, 600, 300), 5)
    pygame.draw.rect(screen, COLORS['black'], (0, 300, 600, 300), 5)


def draw_choose():
    if cur_i < 6:
        pygame.draw.rect(screen, COLORS['blue'], (cur_j * 100 + 5, cur_i * 100 + 5, 100 - 10, 100 - 10), 0)


def check_color(mat, i, j):
    if check(mat, i, j, mat[i][j]):
        return COLORS['green']
    return COLORS['red']


def draw_number():
    for i in range(len(matrix)):
        for j in range(len(matrix[0])):
            if matrix[i][j] == -1:
                continue
            if (i,j) not in blank_ij:
                _color = COLORS['gray']
            else:
                _color = check_color(matrix, i, j)
            txt = font60.render(str(matrix[i][j] if matrix[i][j] in ['1','0',1,0] else ''), True, _color)
            xx, yy = j * 100 + 30, i * 100 + 10
            screen.blit(txt, (xx, yy))


def check_win():
    # if ans_mat == mat:
    #     return True
    # else:
    #     return False
    for (i,j) in blank_ij:
        if matrix[i][j] == -1:
            return False
        elif not check(matrix, i, j, matrix[i][j]):
            return False

    return True


def cheat():
    for (_x, _y) in blank_ij:
        matrix[_x][_y] = ans_matrix[_x][_y]
    return None


def reset():
    for (_x, _y) in blank_ij:
        matrix[_x][_y] = -1


def tip(situation):
    if situation == 1:
        win32api.MessageBox(0, "你太牛了，居然做出来了", "Accepted", win32con.MB_OK)
    else:
        win32api.MessageBox(0, "大哥/姐，你做的不对", "Wrong Answer",win32con.MB_ICONWARNING)
    return None


if __name__ == "__main__":
    # init
    start = 0
    my_answer = 0
    num = 0
    pygame.init()
    pygame.display.set_caption("My Binary Puzzle")
    SIZE = [600, 700]
    screen = pygame.display.set_mode(SIZE)
    # button set
    button_go = Button(screen,"Start",1)
    button_exit = Button(screen,"Exit",0)
    button_cheat = Button(screen,'Help me',2)
    button_reset = Button(screen,'Reset',3)
    button_check = Button(screen,'Check',4)

    background = pygame.image.load(r'C:\Users\霖\Pictures\Jisoo\v2-f3049b290ddcd15a0d3fe4544a4c7310_720w.jpg')
    cur_i, cur_j = 0, 0

    # 系统字体库创建font对象，字体为Times
    font60 = pygame.font.SysFont('Times', 50)
    font70 = pygame.font.SysFont('Times', 60)

    draw_background()
    if not start:
        button_go.draw_button()
        button_exit.draw_button()

    pygame.display.flip()
    pre_run = True
    while pre_run:
        for event in pygame.event.get():
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x,mouse_y = pygame.mouse.get_pos()
                if 75 <= mouse_x <= 225 and 625 <= mouse_y <= 675:
                    pre_run = False
                elif 375 <= mouse_x <= 525 and 625 <= mouse_y <= 675:
                    pygame.quit()
            elif event.type == pygame.QUIT:
                pygame.quit()

    search()
    matrix, ans_matrix, blank_ij = Generate(num)
    num += 1
    # print(matrix,"\n")
    # print(ans_matrix,"\n")

    # loop
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                break
            elif event.type == pygame.MOUSEBUTTONDOWN:
                x, y = pygame.mouse.get_pos()
                cur_j, cur_i = int(x / 100), int(y / 100)
                if 25 <= x <= 175 and 625 <= y <= 675:
                    cheat()
                elif 425 <= x <= 575 and 625 <= y <= 675:
                    reset()
                elif 225 <= x <= 375 and 625 <= y <= 675:
                    if check_win():
                        tip(1)
                        matrix, ans_matrix, blank_ij = Generate(num)
                        num += 1
                    else:
                        tip(2)

            elif event.type == pygame.KEYUP:
                if chr(event.key) in ['1', '0'] and (cur_i, cur_j) in blank_ij:
                    matrix[cur_i][cur_j] = int(chr(event.key))
                else:
                    continue

        # background
        draw_background()
        # literal background
        draw_choose()
        button_cheat.draw_button()
        button_reset.draw_button()
        button_check.draw_button()
        draw_number()
        pygame.display.flip()
