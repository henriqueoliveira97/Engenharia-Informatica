Este projeto teve como objetivo criar um motor de pesquisa de páginas Web. Pretendeu-se que
tenha funcionalidades semelhantes aos serviços Google.com, Qwant.com e Ecosia.org, incluindo
indexação automática (Web crawler) e busca (search engine). O sistema apresenta diversas informações relevantes sobre cada página, tais como o URL, o título, uma citação de texto entre outras. Ao realizar uma busca, obtém-se a lista de páginas que contenham as palavras pesquisadas ordenada por relevância. Os utilizadores podem sugerir URLs para serem indexados pelo sistema. Também é utilizada uma API do Gemini para acrescentar à página de resultados do Googol uma análise textual baseada nos termos da pesquisa e/ou nas citações curtas dos resultados da pesquisa.


Para a execução do nosso projeto serão necessários os seguintes comandos:

*Nota:
Os comandos de execução começados com java tem que ser executados em diferentes terminais

para compilar:

cd chat-app-incomplete/src/main/java
javac -d ../../../target/ -cp ../../../target/lib/jsoup-1.18.3.jar search/*.java pt/uc/sd/MessagingCallBack.java


para executar:

1.

cd chat-app-incomplete/target
java -cp "./lib/jsoup-1.18.3.jar;." search.IndexServer
java -cp "./lib/jsoup-1.18.3.jar;." search.Robot
java -cp "./lib/jsoup-1.18.3.jar;." search.Barrel


2.
cd chat-app-incomplete
./mvnw.cmd spring-boot:run

