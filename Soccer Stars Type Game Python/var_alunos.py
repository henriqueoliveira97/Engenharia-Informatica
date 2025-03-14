#TRABALHO REALIZADO POR:
#Henrique Oliveira, n2022211169
#Simao Cavaleiro, n2023221527



import foosball_alunos

def le_replay(nome_ficheiro):
    
        
        #vai criar uma lista com a divisória no ';' e de seguedida outra lista dentro da mesma coma a divisória no ',' 
        with open(nome_ficheiro,'r') as f:
                vbola = f.readline()
                vjvermelho = f.readline()
                vjazul = f.readline()
                
                vbola = vbola.split(';')
                vjvermelho = vjvermelho.split(';')
                vjazul = vjazul.split(';')
                
                pos_bola = list()
                pos_verm = list()
                pos_azul = list()
                
                div_bola = list()
                div_verm = list()
                div_azul = list()
                
                #range com '-1' para não contar o caracter '\n'
                for i in range(len(vbola)-1):
                        div_bola = vbola[i].split(',')
                        
                        div_verm = vjvermelho[i].split(',')
                        
                        div_azul = vjazul[i].split(',')
                        
                        pos_bola.append((float(div_bola[0]),float(div_bola[1])))
                        pos_verm.append((float(div_verm[0]),float(div_verm[1])))
                        pos_azul.append((float(div_azul[0]),float(div_azul[1])))

               
        
        return {'bola':pos_bola,'jogador_vermelho':pos_verm,'jogador_azul':pos_azul}


def main():
        
        nome='replay_golo_jv_'
        n=int(input('Pontuação do jogador vermelho: '))
        continuacao='_ja_'
        r=int(input('Pontuação do jogador azul: '))
        final='.txt'
        nomefich= nome+str(n)+continuacao+str(r)+final   
        
        estado_jogo = foosball_alunos.init_state()
        foosball_alunos.setup(estado_jogo, False)
        replay = le_replay(nomefich)
        for i in range(len(replay['bola'])):
                estado_jogo['janela'].update()
                estado_jogo['jogador_vermelho'].setpos(replay['jogador_vermelho'][i])
                estado_jogo['jogador_azul'].setpos(replay['jogador_azul'][i])
                estado_jogo['bola']['objecto'].setpos(replay['bola'][i])
        estado_jogo['janela'].exitonclick()


if __name__ == '__main__':
        main()