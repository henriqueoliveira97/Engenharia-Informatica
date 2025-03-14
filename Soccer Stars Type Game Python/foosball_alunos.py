#TRABALHO REALIZADO POR:
#Henrique Oliveira, n2022211169
#Simao Cavaleiro, n2023221527



import turtle as t
import functools
import random
import math

LARGURA_JANELA = 1024
ALTURA_JANELA = 600
DEFAULT_TURTLE_SIZE = 40
DEFAULT_TURTLE_SCALE = 3
RAIO_JOGADOR = DEFAULT_TURTLE_SIZE / DEFAULT_TURTLE_SCALE
RAIO_BOLA = DEFAULT_TURTLE_SIZE / 2
PIXEIS_MOVIMENTO = 90
LADO_MAIOR_AREA = ALTURA_JANELA / 3
LADO_MENOR_AREA = 50
RAIO_MEIO_CAMPO = LADO_MAIOR_AREA / 4
START_POS_BALIZAS = ALTURA_JANELA / 4
BOLA_START_POS = (5,5)



def jogador_cima(estado_jogo, jogador):
    #player é a turtle azul e vermelha
    player = estado_jogo[jogador] 
    if player.ycor() < ALTURA_JANELA / 2 - PIXEIS_MOVIMENTO:
        player.seth(90)
        player.fd(PIXEIS_MOVIMENTO) 
        
    
def jogador_baixo(estado_jogo, jogador):
    player = estado_jogo[jogador]
    if player.ycor() > -ALTURA_JANELA / 2 + PIXEIS_MOVIMENTO:
        player.seth(-90)
        player.fd(PIXEIS_MOVIMENTO)
        
    
def jogador_direita(estado_jogo, jogador):
    player = estado_jogo[jogador]
    if player.xcor() < LARGURA_JANELA / 2 - PIXEIS_MOVIMENTO:
        player.seth(0)    
        player.fd(PIXEIS_MOVIMENTO)
    else:
        estado_jogo[jogador].seth(0)
        estado_jogo[jogador].fd((LARGURA_JANELA/2) - (player.xcor() + (RAIO_JOGADOR*2.5)))        
        
    
def jogador_esquerda(estado_jogo, jogador):
    player = estado_jogo[jogador]
    if player.xcor() > -LARGURA_JANELA / 2 + PIXEIS_MOVIMENTO:
        player.seth(180)
        player.fd(PIXEIS_MOVIMENTO)  
    else:
        estado_jogo[jogador].seth(180)
        estado_jogo[jogador].fd((LARGURA_JANELA/2) + (player.xcor() - (RAIO_JOGADOR*2.5)))        
    

def desenha_linhas_campo():
    #'tl' é a turtle que vai desenhasr as linhas do campo
    tl = t.Turtle()
    tl.pensize(RAIO_BOLA/4)
    tl.pencolor('white')
    
    #limitação do campo
    tl.penup()
    tl.goto(0, ALTURA_JANELA / 2)
    tl.pendown()
    tl.forward(LARGURA_JANELA/2)
    tl.right(90)
    tl.forward(ALTURA_JANELA)
    tl.right(90)
    tl.forward(LARGURA_JANELA)
    tl.right(90)
    tl.fd(ALTURA_JANELA)
    tl.right(90)
    tl.forward(LARGURA_JANELA/2)
    
    #meiocampo
    tl.up()
    tl.goto(0,-(RAIO_MEIO_CAMPO))
    tl.pd()
    tl.circle(RAIO_MEIO_CAMPO)
    tl.up()
    tl.goto(0,ALTURA_JANELA/2)
    tl.pd()    
    tl.goto(0,-ALTURA_JANELA/2)
    tl.up()
    
    #baliza esquerda
    tl.goto(-(LARGURA_JANELA/2),LADO_MAIOR_AREA/2)
    tl.pd()
    tl.fd(LADO_MENOR_AREA)
    tl.rt(90)
    tl.fd(LADO_MAIOR_AREA)
    tl.rt(90)
    tl.fd(LADO_MENOR_AREA)
    tl.rt(90)
    tl.fd(LADO_MAIOR_AREA)
    tl.lt(90)
    
    tl.up()
    
    #baliza direita
    tl.goto((LARGURA_JANELA/2),LADO_MAIOR_AREA/2)
    tl.pd()
    tl.fd(LADO_MENOR_AREA)
    tl.lt(90)
    tl.fd(LADO_MAIOR_AREA)
    tl.lt(90)
    tl.fd(LADO_MENOR_AREA)
    tl.lt(90)
    tl.fd(LADO_MAIOR_AREA)    
    
    tl.ht()


# direção inicial da bola    
def inicial_dir():
    
    dir_x = 0
    dir_y = 0
    
    while dir_x == 0 or dir_y == 0: 
        dir_x = random.uniform(-1,1)    
        dir_y = random.uniform(-1,1)

    magnitude = math.sqrt(dir_x**2 + dir_y**2)
    dir_x /= magnitude
    dir_y /= magnitude
    
    return (dir_x,dir_y)


def criar_bola():
    # direção inicial é sempre aleatoria, devolve o dicionario
    tbola = t.Turtle()
    tbola.up()
    pos_anterior = None
    tbola.fillcolor("black")
    tbola.shape("circle")
    tbola.goto(BOLA_START_POS) 
    
    dir_x,dir_y = inicial_dir()
  
    return {'objecto': tbola, 'dir_x': dir_x, 'dir_y': dir_y, 'pos_anterior': pos_anterior}



def cria_jogador(x_pos_inicial, y_pos_inicial, cor):
    
    #player = criação do jogador azul/vemelho
    player = t.Turtle()
    player.up()
    player.fillcolor(cor)
    player.goto(x_pos_inicial,y_pos_inicial)
    player.shape("circle")
    player.shapesize(DEFAULT_TURTLE_SCALE, DEFAULT_TURTLE_SCALE)
   
    #resgisto dos jogadores
    return player

 
def init_state():
    estado_jogo = {}
    estado_jogo['bola'] = None
    estado_jogo['jogador_vermelho'] = None
    estado_jogo['jogador_azul'] = None
    estado_jogo['var'] = {
        'bola' : [],
        'jogador_vermelho' : [],
        'jogador_azul' : [],
    }
    estado_jogo['pontuacao_jogador_vermelho'] = 0
    estado_jogo['pontuacao_jogador_azul'] = 0
    return estado_jogo

def cria_janela():
    #create a window and declare a variable called window and call the screen()
    window=t.Screen()
    window.title("Foosball Game")
    window.bgcolor("green")
    window.setup(width = LARGURA_JANELA,height = ALTURA_JANELA)
    window.tracer(0)
    return window


def cria_quadro_resultados():
    #Code for creating pen for scorecard update
    quadro=t.Turtle()
    quadro.speed(0)
    quadro.color("Blue")
    quadro.penup()
    quadro.hideturtle()
    quadro.goto(0,260)
    quadro.write("Player A: 0\t\tPlayer B: 0 ", align="center", font=('Monaco',24,"normal"))
    return quadro


def terminar_jogo(estado_jogo):
    print("Adeus")
    estado_jogo['janela'].bye()
    
    # verifica se existe algo escrito, caso não exista, a primeira linha será a indentificação do que cada numero significa
    with open('historico_resultados.csv', 'a+') as f:
        f.seek(0, 2)
        verificar = f.tell()
        linho=f.readlines()

        if verificar == 0:
            f.write('NJogos,JogadorVermelho,JogadorAzul\n')
            num_jogos_total = 1
        else:
            f.seek(0)
            linhas = f.readlines()
            num_jogos_total = len(linhas)  
            
        nova_linha = (str(num_jogos_total)+','+str(estado_jogo['pontuacao_jogador_vermelho'])+','+str(estado_jogo['pontuacao_jogador_azul'])+'\n')
        linho.append(nova_linha)
        f.writelines(linho)        
    
    
def setup(estado_jogo, jogar):
    janela = cria_janela()
    #Assign keys to play
    janela.listen()
    if jogar:
        janela.onkeypress(functools.partial(jogador_cima, estado_jogo, 'jogador_vermelho') ,'w')
        janela.onkeypress(functools.partial(jogador_baixo, estado_jogo, 'jogador_vermelho') ,'s')
        janela.onkeypress(functools.partial(jogador_esquerda, estado_jogo, 'jogador_vermelho') ,'a')
        janela.onkeypress(functools.partial(jogador_direita, estado_jogo, 'jogador_vermelho') ,'d')
        janela.onkeypress(functools.partial(jogador_cima, estado_jogo, 'jogador_azul') ,'Up')
        janela.onkeypress(functools.partial(jogador_baixo, estado_jogo, 'jogador_azul') ,'Down')
        janela.onkeypress(functools.partial(jogador_esquerda, estado_jogo, 'jogador_azul') ,'Left')
        janela.onkeypress(functools.partial(jogador_direita, estado_jogo, 'jogador_azul') ,'Right')
        janela.onkeypress(functools.partial(reset_pos, estado_jogo) ,'r')
        janela.onkeypress(functools.partial(terminar_jogo, estado_jogo) ,'Escape')
        quadro = cria_quadro_resultados()
        estado_jogo['quadro'] = quadro
    desenha_linhas_campo()
    bola = criar_bola()
    jogador_vermelho = cria_jogador(-((ALTURA_JANELA / 2) + LADO_MENOR_AREA), 0, "red")
    jogador_azul = cria_jogador(((ALTURA_JANELA / 2) + LADO_MENOR_AREA), 0, "blue")
    estado_jogo['janela'] = janela
    estado_jogo['bola'] = bola
    estado_jogo['jogador_vermelho'] = jogador_vermelho
    estado_jogo['jogador_azul'] = jogador_azul
    #botão de reset no 'r' é adicional
    
def update_board(estado_jogo):
    estado_jogo['quadro'].clear()
    estado_jogo['quadro'].write("Player A: {}\t\tPlayer B: {} ".format(estado_jogo['pontuacao_jogador_vermelho'], estado_jogo['pontuacao_jogador_azul']),align="center",font=('Monaco',24,"normal"))

def movimenta_bola(estado_jogo):
    #movimento = posição da bola mais direção 
    guarda_posicoes_para_var(estado_jogo)
    tbola = estado_jogo['bola']['objecto']
    estado_jogo['bola']['pos_anterior'] = tbola.pos()
    
    x, y = tbola.pos()
    dir_x, dir_y = estado_jogo['bola']['dir_x'], estado_jogo['bola']['dir_y']

    novo_x = (x + (dir_x*1.5)) 
    novo_y = (y + (dir_y*1.5))     
    
    tbola.goto(novo_x, novo_y)
   

def verifica_colisoes_ambiente(estado_jogo):
    pos_x,pos_y = estado_jogo['bola']['objecto'].pos()
    x = estado_jogo['bola']['dir_x']
    y = estado_jogo['bola']['dir_y']
    
    if abs(pos_y) >= ALTURA_JANELA/2 - RAIO_BOLA/2:
        y = -y 
    
    if LARGURA_JANELA/2 - RAIO_BOLA/2 <= abs(pos_x) < LARGURA_JANELA/2 + RAIO_BOLA/2:
            if abs(pos_y) >= LADO_MAIOR_AREA/2 + RAIO_BOLA/4 :
                x = -x
            elif LADO_MAIOR_AREA/2 - RAIO_BOLA/4 <= abs(pos_y) < LADO_MAIOR_AREA/2 + RAIO_BOLA/4:
                y = -y
                
        
    estado_jogo['bola']['dir_x'] = x
    estado_jogo['bola']['dir_y'] = y       


def verifica_golo_jogador_vermelho(estado_jogo):
    
    tbola = estado_jogo['bola']['objecto']
    
    if (tbola.xcor() >= LARGURA_JANELA/2 + RAIO_BOLA/2):
        estado_jogo['pontuacao_jogador_vermelho'] += 1  
        update_board(estado_jogo)
        ficheiro_replay_golo(estado_jogo)
        reset_pos(estado_jogo)
   

def verifica_golo_jogador_azul(estado_jogo):
    
    tbola = estado_jogo['bola']['objecto']
    
    if tbola.xcor() <= (-LARGURA_JANELA/2 - RAIO_BOLA):
        estado_jogo['pontuacao_jogador_azul'] += 1
        update_board(estado_jogo)
        ficheiro_replay_golo(estado_jogo)
        reset_pos(estado_jogo)
        
        

def reset_pos(estado_jogo):
    estado_jogo['jogador_vermelho'].goto(-((ALTURA_JANELA / 2) + LADO_MENOR_AREA),0)
    estado_jogo['jogador_azul'].goto((ALTURA_JANELA / 2) + LADO_MENOR_AREA,0)
    estado_jogo['bola']['objecto'].goto(BOLA_START_POS)
    
    dir_x,dir_y = inicial_dir()
    
    estado_jogo['bola']['dir_x'] = dir_x
    estado_jogo['bola']['dir_y'] = dir_y
    
    


def verifica_golos(estado_jogo):
    verifica_golo_jogador_vermelho(estado_jogo)
    verifica_golo_jogador_azul(estado_jogo)
    


def verifica_toque_jogador_azul(estado_jogo):
    
    tbola = estado_jogo['bola']['objecto']
    player = estado_jogo['jogador_azul']
    
    if player.distance(tbola) < (RAIO_JOGADOR + RAIO_BOLA + RAIO_JOGADOR/2):
        #O + RAIO_JOGADOR/2 serve para ter precisão exata
        
        #normal_x + normal_y = vetor com origem na bola e direção ao centro jogador
        normal_x = player.xcor() - tbola.xcor()
        normal_y = player.ycor() - tbola.ycor()
        
        #magnitude = distância,  serve para a normalização do vetor 
        magnitude = math.sqrt(normal_x**2 + normal_y**2)
        normal_x /= magnitude
        normal_y /= magnitude
  
        #É necessesário inverter a direção do vetor, para a bola refletir quando atingir o jogador
        estado_jogo['bola']['dir_x'] = -normal_x
        estado_jogo['bola']['dir_y'] = -normal_y    
        




def verifica_toque_jogador_vermelho(estado_jogo):
    
    tbola = estado_jogo['bola']['objecto']
    player = estado_jogo['jogador_vermelho']

    
    if player.distance(tbola) < (RAIO_JOGADOR + RAIO_BOLA + RAIO_JOGADOR/2):    
        #O + RAIO_JOGADOR/2 serve para ter uma precisão exata
        
        #normal_x + normal_y = vetor com origem na bola e direção ao centro jogador
        normal_x = player.xcor() - tbola.xcor()
        normal_y = player.ycor() - tbola.ycor()
        
        #magnitude = distância,  serve para a normalização do vetor
        magnitude = math.sqrt(normal_x**2 + normal_y**2)
        normal_x /= magnitude
        normal_y /= magnitude
        
        #É necessesário inverter a direção do vetor, para a bola refletir quando atingir o jogador
        estado_jogo['bola']['dir_x'] = -normal_x
        estado_jogo['bola']['dir_y'] = -normal_y    
        

def guarda_posicoes_para_var(estado_jogo):
    estado_jogo['var']['bola'].append(estado_jogo['bola']['objecto'].pos())
    estado_jogo['var']['jogador_vermelho'].append(estado_jogo['jogador_vermelho'].pos())
    estado_jogo['var']['jogador_azul'].append(estado_jogo['jogador_azul'].pos())



def ficheiro_replay_golo(estado_jogo):
    num_golos_vermelho = estado_jogo['pontuacao_jogador_vermelho']
    num_golos_azul = estado_jogo['pontuacao_jogador_azul']
    nome_fich = "replay_golo_jv_" + str(num_golos_vermelho) + "_ja_" + str(num_golos_azul) + ".txt"


    fich=open(nome_fich, 'w')
        # Escrever as coordenadas da bola
    for coord in estado_jogo['var']['bola']:
        fich.write(str(coord[0]) + "," + str(coord[1]) + ";")
    fich.write('\n')

        # Escrever as coordenadas do jogador vermelho
    for coord in estado_jogo['var']['jogador_vermelho']:
        fich.write(str(coord[0]) + "," + str(coord[1]) + ";")
    fich.write('\n')
    
    # Escrever as coordenadas do jogador azul
    for coord in estado_jogo['var']['jogador_azul']:
        fich.write(str(coord[0]) + "," + str(coord[1]) + ";")
    fich.write('\n')
    
    fich.close()
        
def main():
    estado_jogo = init_state()
    setup(estado_jogo, True)
    while True:
        estado_jogo['janela'].update()
        if estado_jogo['bola'] is not None:
            movimenta_bola(estado_jogo)
        verifica_colisoes_ambiente(estado_jogo)
        verifica_golos(estado_jogo)
        if estado_jogo['jogador_vermelho'] is not None:
            verifica_toque_jogador_azul(estado_jogo)
        if estado_jogo['jogador_azul'] is not None:
            verifica_toque_jogador_vermelho(estado_jogo)

if __name__ == '__main__':
    main()