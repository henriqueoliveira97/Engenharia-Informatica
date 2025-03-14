import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.io.*;

/**
 * Classe principal da aplicação, responsável por gerir clientes, produtos, faturas e suas operações.
 * Fornece métodos para adicionar, listar, alterar e gerir dados, bem como ler e escrever em ficheiros.
 */
public class Aplicacao {
    private ArrayList<Cliente> clientes;
    private ArrayList<Fatura> faturas;
    private ArrayList<ProdAlimentares> produtosA;
    private ArrayList<ProdFarmacia> produtosF;
    private Verificador verificador;
    /**
     * Construtor da classe Aplicacao. Inicializa as listas e o verificador.
     */
    public Aplicacao() {
        this.clientes = new ArrayList<>();
        this.faturas = new ArrayList<>();
        this.produtosA = new ArrayList<>();
        this.produtosF = new ArrayList<>();
        this.verificador  = new Verificador();
    }

    /**
     * Adiciona um novo cliente ao sistema, solicitando informações ao utilizador.
     * Valida os dados antes de adicioná-los à lista de clientes.
     */
    public void adicionarCliente() {
        Scanner scanner = new Scanner(System.in);
        String nome, nContribuinte, localizacao;

        while (true) {
            System.out.println("Nome (ou digite '0' para sair): ");
            nome = scanner.nextLine();
            if (nome.equals("0")) {
                System.out.println("Operação cancelada.");
                return;
            }

            System.out.println("Número de Contribuinte: ");
            nContribuinte = scanner.nextLine();

            System.out.println("Localização (Portugal Continental, Madeira, Açores):");
            localizacao = scanner.nextLine();

            if (verificador.verify(localizacao, nContribuinte, nome, clientes)) {
                Cliente cliente = new Cliente(nome, nContribuinte, localizacao);
                this.clientes.add(cliente);
                System.out.println("Cliente " + nome + " adicionado!");
                atualizaClientes(nome, localizacao, nContribuinte);
                break;
            } else {
                System.out.println("Dados inválidos. Por favor, tente novamente.");
            }
        }
    }

    /**
     * Lista todos os clientes cadastrados, exibindo suas informações.
     */
    public void listarClientes(){
        System.out.println(" ------------");
        System.out.println("|   CLIENTES   |");
        System.out.println(" ------------");
        for(Cliente cliente: clientes){
            System.out.println(cliente);
        }
        System.out.println(" ------------");

    }

    /**
     * Lê produtos a partir de um ficheiro "produtos.txt" e adiciona-os à lista de produtos.
     * Cada produto é verificado antes de ser adicionado.
     */
    public void adicionarProduto() {
        String caminhoDoFicheiro = "produtos.txt";

        try (BufferedReader leitor = new BufferedReader(new FileReader(caminhoDoFicheiro))) {
            String linha;
            while ((linha = leitor.readLine()) != null) {
                String[] dados = linha.split("\\|");

                if (dados.length < 10) {
                    System.out.println("Produto mal formatado: " + linha);
                    continue;
                }

                String tipoProduto = dados[9].trim().toLowerCase();

                String nome = dados[0].trim();
                String codigo = dados[1].trim();
                String descricao = dados[2].trim();
                int quantidade = Integer.parseInt(dados[3].trim());
                double valorUnitario = Double.parseDouble(dados[4].trim().replace(",", "."));
                String categoria = dados[7].trim();
                String prodBio = dados[8].trim();

                if (tipoProduto.equals("alimentar")) {
                    String taxa = dados[5].trim();
                    String[] certificacao = dados[6].trim().split(",");

                    if (verificador.verifyProdA(codigo, descricao, quantidade, valorUnitario, taxa, categoria, produtosA)) {
                        ProdAlimentares prodA = new ProdAlimentares(nome, codigo, descricao, quantidade, valorUnitario, 0, prodBio, taxa, certificacao, categoria);
                        produtosA.add(prodA);
                    }
                } else if (tipoProduto.equals("farmacia")) {
                    String prescricaoTexto = dados[5].trim().toUpperCase();
                    String medico = dados[6].trim();
                    boolean prescricao= false;

                    if (prescricaoTexto.equals("SIM")) {
                        prescricao = true;
                    } else if (prescricaoTexto.equals("NAO")) {
                        prescricao = false;
                    } else {
                        System.out.println("Erro: Valor inválido para prescrição no produto: " + nome);
                    }

                    if (verificador.verifyProdF(codigo, descricao, quantidade, valorUnitario, medico, categoria, produtosF)) {
                        ProdFarmacia prodF = new ProdFarmacia(nome, codigo, descricao, quantidade, valorUnitario, 0, prodBio, prescricao, medico, categoria);
                        produtosF.add(prodF);
                    }
                }
            }
        } catch (IOException e) {
            System.out.println("Ocorreu um erro ao ler o ficheiro");
        } catch (NumberFormatException e) {
            System.out.println("Erro ao converter número");
        }
    }

    /**
     * Cria uma nova fatura para um cliente existente, permitindo adicionar produtos disponíveis.
     * Valida as entradas e atualiza os ficheiros de faturas.
     */
    public void adicionarFatura() {

        Scanner scanner = new Scanner(System.in);

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNúmero da Fatura: ");
        String nFatura = scanner.nextLine();

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tInsira o índice do cliente:");
        for (int i = 0; i < clientes.size(); i++) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"+i + " - " + clientes.get(i).getnContribuinte() + " (" + clientes.get(i).getNome() + ")");
        }
        int clienteIndex = scanner.nextInt();
        scanner.nextLine();
        if (clienteIndex < 0 || clienteIndex >= clientes.size()) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tÍndice de cliente inválido!");
            return;
        }
        Cliente cliente = clientes.get(clienteIndex);

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tData (dd/MM/yyyy): ");
        String data = scanner.nextLine();

        if (!verificador.verifyFaturas(nFatura, cliente, data,clientes,faturas)) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFatura inválida!");
            return;
        }
        Fatura fatura = new Fatura(nFatura, cliente, data);

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tProdutos disponíveis:");
        for (int i = 0; i < produtosA.size(); i++) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tAlimentar - " + i + " - " + produtosA.get(i).getNome() + " (" + produtosA.get(i).getDescricao() + ")");
        }
        for (int i = 0; i < produtosF.size(); i++) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFarmácia - " + (i + produtosA.size()) + " - " + produtosF.get(i).getNome() + " (" + produtosF.get(i).getDescricao() + ")");
        }

        while (true) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tInsira o índice do produto a adicionar à fatura (ou -1 para concluir):");
            int produtoIndex = scanner.nextInt();
            scanner.nextLine();
            if (produtoIndex == -1) {
                break;
            }

            if (produtoIndex >= 0 && produtoIndex < produtosA.size()) {
                ProdAlimentares prodA = produtosA.get(produtoIndex);
                fatura.adicionarProduto(prodA);
            } else if (produtoIndex >= produtosA.size() && produtoIndex < (produtosA.size() + produtosF.size())) {
                ProdFarmacia prodF = produtosF.get(produtoIndex - produtosA.size());
                fatura.adicionarProduto(prodF);
            } else {
                System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tÍndice inválido! Tente novamente.");
            }
        }

        faturas.add(fatura);
        atualizarFicheiroFaturas(fatura);

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFatura criada com sucesso!");
    }

    /**
     * Atualiza o ficheiro de faturas com uma nova fatura.
     *
     * @param fatura Fatura a ser adicionada ao ficheiro.
     */
    private void atualizarFicheiroFaturas(Fatura fatura) {
        String caminhoDoFicheiro = "faturas.txt";
        try (BufferedWriter escritor = new BufferedWriter(new FileWriter(caminhoDoFicheiro, true))) {
            // Escrevendo a fatura manualmente
            escritor.write(fatura.getnFatura() + ":");
            escritor.write(fatura.getCliente().getNome() + ":");
            escritor.write(fatura.getCliente().getnContribuinte() + ":");
            escritor.write(fatura.getData() + ":(");
            List<Produto> produtos = fatura.getProdutos();
            for (int i = 0; i < produtos.size(); i++) {
                Produto produto = produtos.get(i);
                escritor.write(produto.toString());
                if (i < produtos.size() - 1) {
                    escritor.write(";");
                }
            }
            escritor.write("):");
            escritor.write(String.format("%.2f:", fatura.calcularTotalSemIVA()));
            escritor.write(String.format("%.2f", fatura.calcularTotalComIVA()));
            escritor.newLine();
            System.out.println("Fatura registada no ficheiro com sucesso.");
        } catch (IOException e) {
            System.out.println("Ocorreu um erro ao registar a fatura no ficheiro" );
        }
    }

    /**
     * Importa faturas a partir de um ficheiro "faturas.txt".
     * Associa os produtos e clientes às faturas importadas, validando seus dados.
     */
    public void importarFaturas() {
        String caminhoFicheiro = "faturas.txt";
        try (BufferedReader leitor = new BufferedReader(new FileReader(caminhoFicheiro))) {
            String linha;

            while ((linha = leitor.readLine()) != null) {
                String[] partes = linha.split(":");

                String nFatura = partes[0];

                String nomeCliente = partes[1];
                String numContribuinte = partes[2];

                Cliente cliente = null;
                for (Cliente c : clientes) {
                    if (c.getnContribuinte().equalsIgnoreCase(numContribuinte)) {
                        cliente = c;
                        break;
                    }
                }

                if (cliente == null) {
                    System.out.println("Cliente " + nomeCliente + " não encontrado. Ignorando fatura.");
                    continue;
                }

                String data = partes[3];

                String produtosTexto = partes[4].substring(1, partes[4].length() - 1);
                String[] produtosArray = produtosTexto.split(";");

                ArrayList<Produto> produtos = new ArrayList<>();

                for (String produtoTexto : produtosArray) {
                    String[] detalhesProduto = produtoTexto.split("\\|");
                    String nomeProduto = detalhesProduto[0];
                    String codigo = detalhesProduto[1];
                    String descricao = detalhesProduto[2];
                    int quantidade = Integer.parseInt(detalhesProduto[3]);
                    double valorUnitario = Double.parseDouble(detalhesProduto[3]);

                    Produto produtoEncontrado = null;
                    for (ProdAlimentares prodA : produtosA) {
                        if (prodA.getCodigo().equalsIgnoreCase(codigo)) {
                            produtoEncontrado = prodA;
                            break;
                        }
                    }
                    if (produtoEncontrado == null) {
                        for (ProdFarmacia prodF : produtosF) {
                            if (prodF.getCodigo().equalsIgnoreCase(codigo)) {
                                produtoEncontrado = prodF;
                                break;
                            }
                        }
                    }

                    if (produtoEncontrado != null) {
                        produtoEncontrado.setQuantidade(quantidade);
                        produtos.add(produtoEncontrado);
                    } else {
                        System.out.println("Produto " + nomeProduto + " não encontrado. Ignorando.");
                    }
                }


                Fatura fatura = new Fatura(nFatura, cliente, data);
                for (Produto p : produtos) {
                    fatura.adicionarProduto(p);
                }

                faturas.add(fatura);
                System.out.println("Fatura importada: " + nFatura);
            }
        } catch (IOException e) {
            System.out.println("Erro ao ler o ficheiro de faturas");
        } catch (Exception e) {
            System.out.println("Erro no formato da fatura");
        }
    }

    /**
     * Lista todas as faturas registradas, exibindo suas informações básicas.
     */
    public void listarFaturas(){
        for(Fatura fatura: faturas){
            fatura.listarFatura();
        }
    }

    /**
     * Exibe os detalhes completos de todas as faturas, incluindo produtos e valores.
     */
    public void visualizarFaturas(){
        for (Fatura fatura: faturas){
            fatura.visualizarFatura();
        }
    }

    /**
     * Atualiza o ficheiro de clientes com novas informações de um cliente.
     *
     * @param nome Nome do cliente.
     * @param localizacao Localização do cliente.
     * @param nContribuinte Número de contribuinte do cliente.
     */
    public void atualizaClientes(String nome, String localizacao, String nContribuinte) {
        String caminhoDoFicheiro = "clientes.txt";
        String conteudoAdicional = localizacao + '|' + nome + '|' + nContribuinte+ '\n';
        try (BufferedWriter escritor = new BufferedWriter(new FileWriter(caminhoDoFicheiro, true))) {
            escritor.write(conteudoAdicional);
            escritor.newLine();
            System.out.println("Conteúdo adicional escrito no ficheiro com sucesso.");
        } catch (IOException e) {
            System.out.println("Erro ao adicionar conteúdo ao ficheiro" );
        }
    }

    /**
     * Lê os clientes a partir de um ficheiro "clientes.txt" e os adiciona à lista de clientes.
     * Cada cliente é validado antes de ser adicionado.
     */
    public void infoFileClients() {
        String caminhoDoFicheiro = "clientes.txt";
        try (BufferedReader leitor = new BufferedReader(new FileReader(caminhoDoFicheiro))) {
            String linha;
            while ((linha = leitor.readLine()) != null) {
                String[] dados = linha.split("\\|");
                if (dados.length == 3) {
                    if (verificador.verify(dados[0], dados[2], dados[1], clientes)) {
                        Cliente cliente = new Cliente(dados[1], dados[2], dados[0]);
                        this.clientes.add(cliente);
                    }
                }
            }
        } catch (IOException e) {
            System.out.println("Ocorreu um erro ao ler o ficheiro ");
        }
    }

    /**
     * Permite alterar as informações de um cliente existente.
     * Atualiza o ficheiro de clientes com as alterações realizadas.
     */
    public void alterarCliente() {
        Scanner scanner = new Scanner(System.in);
        String caminhoDoFicheiro = "clientes.txt";
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tDigite o número de contribuinte do cliente que deseja alterar:");
        String nContribuinte = scanner.nextLine();
        Cliente cliente = null;
        for (Cliente c : clientes) {
            if (c.getnContribuinte().equals(nContribuinte)) {
                cliente = c;
                break;
            }
        }
        if(cliente == null) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCliente não encontrado.");
            return;
        }
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEditando cliente: " + cliente.getNome() + " (" + cliente.getnContribuinte() + ")");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tDeixe o campo em branco para manter o valor atual.");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNovo nome (atual: " + cliente.getNome() + "): ");
        String novoNome = scanner.nextLine();
        if (!novoNome.isEmpty()) {
            cliente.nome = novoNome;
        }
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNova localização (atual: " + cliente.getLocalizacao() + "): ");
        String novaLocalizacao = scanner.nextLine();
        if (!novaLocalizacao.isEmpty()) {
            cliente.localizacao = novaLocalizacao;
        }
        try {
            File ficheiro = new File(caminhoDoFicheiro);
            File ficheiroTemp = new File("clientes_temp.txt");

            try (BufferedReader leitor = new BufferedReader(new FileReader(ficheiro));
                 BufferedWriter escritor = new BufferedWriter(new FileWriter(ficheiroTemp))) {
                String linha;

                while ((linha = leitor.readLine()) != null) {
                    String[] dados = linha.split("\\|");

                    if (dados.length == 3 && dados[2].equals(nContribuinte)) {
                        escritor.write(cliente.localizacao + "|" + cliente.nome + "|" + cliente.nContribuinte);
                    } else {
                        escritor.write(linha);
                    }
                    escritor.newLine();
                }
            }
            if (ficheiro.delete() && ficheiroTemp.renameTo(ficheiro)) {
                System.out.println("Ficheiro atualizado com sucesso.");
            } else {
                System.out.println("Erro ao substituir o ficheiro original.");
            }
        } catch (IOException e) {
            System.out.println("Erro ao manipular o ficheiro: ");
        }
        System.out.println("Informações do cliente atualizadas com sucesso!");
    }

    /**
     * Altera os dados de uma fatura existente, permitindo adicionar ou remover produtos.
     * As alterações são refletidas no ficheiro de faturas.
     */
    public void alterarFatura() {
        Scanner scanner = new Scanner(System.in);
        String caminhoDoFicheiro = "faturas.txt";

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tDigite o número da fatura que deseja alterar:");
        String nFatura = scanner.nextLine();

        Fatura fatura = null;
        for (Fatura f : faturas) {
            if (f.getnFatura().equals(nFatura)) {
                fatura = f;
                break;
            }
        }

        if (fatura == null) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFatura não encontrada.");
            return;
        }

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEditando fatura: " + fatura.getnFatura());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tProdutos atuais na fatura:");
        for (int i = 0; i < fatura.getProdutos().size(); i++) {
            Produto produto = fatura.getProdutos().get(i);
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"+i + " - " + produto.getNome() + " (" + produto.getDescricao() + ")");
        }

        while (true) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEscolha uma ação:");
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t1 - Adicionar produto");
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t2 - Remover produto");
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t3 - Finalizar alterações");
            int opcao = scanner.nextInt();
            scanner.nextLine();

            if (opcao == 1) {
                // Adicionar produto à fatura
                System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tProdutos disponíveis:");
                for (int i = 0; i < produtosA.size(); i++) {
                    System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tAlimentar - " + i + " - " + produtosA.get(i).getNome() + " (" + produtosA.get(i).getDescricao() + ")");
                }
                for (int i = 0; i < produtosF.size(); i++) {
                    System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFarmácia - " + (i + produtosA.size()) + " - " + produtosF.get(i).getNome() + " (" + produtosF.get(i).getDescricao() + ")");
                }

                System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tInsira o índice do produto a adicionar:");
                int produtoIndex = scanner.nextInt();
                scanner.nextLine();
                if (produtoIndex >= 0 && produtoIndex < produtosA.size()) {
                    ProdAlimentares prodA = produtosA.get(produtoIndex);
                    fatura.adicionarProduto(prodA);
                } else if (produtoIndex >= produtosA.size() && produtoIndex < (produtosA.size() + produtosF.size())) {
                    ProdFarmacia prodF = produtosF.get(produtoIndex - produtosA.size());
                    fatura.adicionarProduto(prodF);
                } else {
                    System.out.println("Índice inválido! Tente novamente.");
                }

            } else if (opcao == 2) {
                // Remover produto da fatura
                System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tDigite o índice do produto a remover:");
                int produtoIndex = scanner.nextInt();
                scanner.nextLine();
                if (produtoIndex >= 0 && produtoIndex < fatura.getProdutos().size()) {
                    fatura.getProdutos().remove(produtoIndex);
                    System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tProduto removido.");
                } else {
                    System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tÍndice inválido! Tente novamente.");
                }

            } else if (opcao == 3) {
                // Finalizar alterações
                break;
            } else {
                System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tOpção inválida. Tente novamente.");
            }
        }

        // Atualizar o ficheiro de faturas
        try {
            File ficheiro = new File(caminhoDoFicheiro);
            File ficheiroTemp = new File("faturas_temp.txt");

            try (BufferedReader leitor = new BufferedReader(new FileReader(ficheiro));
                 BufferedWriter escritor = new BufferedWriter(new FileWriter(ficheiroTemp))) {
                String linha;

                while ((linha = leitor.readLine()) != null) {
                    String[] dados = linha.split(":");

                    if (dados[0].equals(fatura.getnFatura())) {
                        // Atualiza a linha da fatura editada
                        escritor.write(fatura.getnFatura() + ":");
                        escritor.write(fatura.getCliente().getNome() + ":");
                        escritor.write(fatura.getCliente().getnContribuinte() + ":");
                        escritor.write(fatura.getData() + ":(");
                        List<Produto> produtos = fatura.getProdutos();
                        for (int i = 0; i < produtos.size(); i++) {
                            Produto produto = produtos.get(i);
                            escritor.write(produto.toString());
                            if (i < produtos.size() - 1) {
                                escritor.write(";");
                            }
                        }
                        escritor.write("):");
                        escritor.write(String.format("%.2f:", fatura.calcularTotalSemIVA()));
                        escritor.write(String.format("%.2f", fatura.calcularTotalComIVA()));
                        escritor.newLine();
                    } else {
                        // Mantém as outras linhas intactas
                        escritor.write(linha);
                        escritor.newLine();
                    }
                }
            }

            if (ficheiro.delete() && ficheiroTemp.renameTo(ficheiro)) {
                System.out.println("Ficheiro de faturas atualizado com sucesso.");
            } else {
                System.out.println("Erro ao substituir o ficheiro original.");
            }

        } catch (IOException e) {
            System.out.println("Erro ao manipular o ficheiro de faturas: ");
        }

        System.out.println("Fatura atualizada com sucesso!");
    }

    /**
     * Exibe as estatísticas da aplicação, incluindo o número total de faturas, produtos e os totais com e sem IVA.
     */
    public void estatisticas() {
        int numeroFaturas = faturas.size();
        int numeroProdutos = 0;
        double totalSemIVA = 0.0;
        double totalIVA = 0.0;
        double totalComIVA = 0.0;

        for (Fatura fatura : faturas) {
            numeroProdutos += fatura.getProdutos().size();
            totalSemIVA += fatura.calcularTotalSemIVA();
            totalIVA += fatura.calcularTotalIVA();
            totalComIVA += fatura.calcularTotalComIVA();
        }

        // Exibir as estatísticas
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t---- Estatísticas ----");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNúmero de faturas: " + numeroFaturas);
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNúmero de produtos: " + numeroProdutos);
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tValor total sem IVA: %.2f€\n", totalSemIVA);
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tValor total do IVA: %.2f€\n", totalIVA);
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tValor total com IVA: %.2f€\n", totalComIVA);
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t----------------------");

    }

    /**
     * Carrega os dados de clientes e faturas a partir de um ficheiro de objetos ("dados.obj").
     * Se o ficheiro não existir ou ocorrer algum erro, os dados são inicializados vazios.
     */
    public void carregarDados() {
        File ficheiro = new File("dados.obj");
        if (ficheiro.exists()) {
            try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(ficheiro))) {
                this.clientes = (ArrayList<Cliente>) in.readObject();
                this.faturas = (ArrayList<Fatura>) in.readObject();
                System.out.println("Dados carregados do ficheiro de objetos.");
            } catch (IOException | ClassNotFoundException e) {
                System.out.println("Erro ao carregar dados do ficheiro de objetos");
                inicializarDados();
            }
        } else {
            System.out.println("Ficheiro não encontrado. Inicializando dados.");
            inicializarDados();
        }
    }

    /**
     * Inicializa as listas de clientes e faturas com dados vazios.
     */
    private void inicializarDados() {
        this.clientes = new ArrayList<>();
        this.faturas = new ArrayList<>();

    }
    /**
     * Salva os dados dos clientes e faturas em um ficheiro de objetos ("dados.obj").
     */
    public void guardarDados() {
        try (ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("dados.obj"))) {
            out.writeObject(this.clientes);
            out.writeObject(this.faturas);
            System.out.println("Dados de clientes e faturas guardados no ficheiro de objetos.");
        } catch (IOException e) {
            System.out.println("Erro ao guardados dados no ficheiro de objetos");
        }
    }

    /**
     * Método principal para rodar a aplicação. Exibe o menu principal e permite ao utilizador interagir com a aplicação.
     * Dependendo da opção escolhida, o utilizador pode adicionar clientes, faturas, alterar dados ou visualizar informações.
     */
    public static void main(String[] args) {
        Aplicacao app = new Aplicacao();
        app.inicializarDados();
        app.adicionarProduto();
        File ficheiro = new File("dados.obj");
        if(ficheiro.exists()) {
            app.carregarDados();
        }else{
            app.infoFileClients();
            app.importarFaturas();
        }
        Scanner scanner = new Scanner(System.in);
        int opcao;
        do{
            System.out.println("--- Menu Principal ---");
            System.out.println("1. Adicionar Cliente");
            System.out.println("2. Alterar Cliente");
            System.out.println("3. Listar Clientes");
            System.out.println("4. Adicionar Fatura");
            System.out.println("5. Listar Faturas");
            System.out.println("6. Visualizar Faturas");
            System.out.println("7. Alterar fatura");
            System.out.println("8. Mostrar Estatísticas");
            System.out.println("0. Sair");
            System.out.println("Escolha uma opção: ");
            opcao = scanner.nextInt();
            scanner.nextLine();

            switch (opcao) {
                case 1:
                    app.adicionarCliente();
                    break;
                case 2:
                    app.alterarCliente();
                    break;
                case 3:
                    app.listarClientes();
                    break;
                case 4:
                    app.adicionarFatura();
                    break;
                case 5:
                    app.listarFaturas();
                    break;
                case 6:
                    app.visualizarFaturas();
                    break;
                case 7:
                    app.alterarFatura();
                    break;
                case 8:
                    app.estatisticas();
                    break;

                case 0:
                    System.out.println("A guardar dados e sair da aplicação...");
                    app.guardarDados();
                    break;
                default:
                    System.out.println("Opção inválida, tente outra vez.");
            }
        } while (opcao != 0);
        scanner.close();
    }
}
