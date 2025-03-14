import java.sql.SQLOutput;
import java.util.ArrayList;
import java.io.Serializable;

/**
 * Representa uma fatura com informações sobre número, cliente, data e produtos associados.
 * Implementa a interface Serializable para que os objetos dessa classe possam ser serializados.
 */
public class Fatura implements Serializable{
    protected String nFatura;
    protected Cliente cliente;
    protected String data;
    protected ArrayList<Produto> produtos;


    /**
     * Construtor da classe Fatura.
     *
     * @param nFatura Número identificador da fatura.
     * @param cliente Cliente associado à fatura.
     * @param data Data de emissão da fatura no formato "dd/MM/yyyy".
     */
    public Fatura(String nFatura, Cliente cliente, String data){
        this.nFatura=nFatura;
        this.cliente= cliente;
        this.data=data;
        produtos=new ArrayList<Produto>();
    }

    /**
     * Obtém o número da fatura.
     *
     * @return Número da fatura.
     */
    public String getnFatura() {
        return nFatura;
    }

    /**
     * Obtém o cliente associado à fatura.
     *
     * @return Cliente associado à fatura.
     */
    public Cliente getCliente() {
        return cliente;
    }

    /**
     * Obtém a data de emissão da fatura.
     *
     * @return Data de emissão da fatura.
     */
    public String getData() {
        return data;
    }

    /**
     * Obtém a lista de produtos incluídos na fatura.
     *
     * @return Lista de produtos.
     */
    public ArrayList<Produto> getProdutos() {
        return produtos;
    }

    /**
     * Adiciona um produto à fatura e define sua taxa de IVA com base na localização do cliente.
     *
     * @param produto Produto a ser adicionado à fatura.
     */
    public void adicionarProduto(Produto produto) {
        produto.defTaxa(cliente.getLocalizacao());
        produtos.add(produto);
    }

    /**
     * Calcula o valor total da fatura sem IVA.
     *
     * @return Valor total sem IVA.
     */
    public double calcularTotalSemIVA() {
        double total = 0.0;
        for (Produto produto : produtos) {
            total += produto.calcularValorSemIVA();
        }
        return total;
    }

    /**
     * Calcula o valor total da fatura com IVA.
     *
     * @return Valor total com IVA.
     */
    public double calcularTotalComIVA() {
        double total = 0.0;
        for (Produto produto : produtos) {
            total += produto.calcularValorComIVA();
        }
        return total;
    }

    /**
     * Calcula o valor total do IVA da fatura.
     *
     * @return Valor total do IVA.
     */
    public double calcularTotalIVA(){
        double total=0.0;
        for(Produto produto: produtos){
            total+= produto.calcularValorIVA();
        }
        return total;
    }

    /**
     * Lista as informações básicas da fatura, como número, cliente, total de produtos e valores.
     */
    public void listarFatura() {
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t ------------");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t|   FATURA   |");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t ------------");

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFatura Nº: " + nFatura);
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCliente: " + cliente.getNome());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tLocalização do Cliente: " + cliente.getLocalizacao());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tNúmero de Produtos: " + produtos.size());
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tValor Total Sem IVA: %.2f€\n", calcularTotalSemIVA());
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tValor Total Com IVA: %.2f€\n", calcularTotalComIVA());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t--------------------------");
        System.out.println("\n");
    }

    /**
     * Mostra uma visão detalhada da fatura, incluindo os produtos e seus valores detalhados.
     */
    public void visualizarFatura() {
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t ------------");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t|   FATURA   |");
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t ------------");

        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tFatura Nº: " + nFatura);
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tData: " + data);
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCliente: " + cliente.getNome());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tLocalização do Cliente: " + cliente.getLocalizacao());
        System.out.println("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tProdutos:");

        for (Produto produto : produtos) {
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t - Produto: " + produto.getNome());
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Quantidade: " + produto.getQuantidade());
            System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Valor Unitário: %.2f€\n", produto.getValorUnitario());
            System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Valor Total Sem IVA: %.2f€\n", produto.calcularValorSemIVA());
            System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Valor Taxa IVA: %.2f \n", produto.getTaxaIVA());
            System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Valor do IVA: %.2f€\n", produto.calcularValorIVA());
            System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   Valor Total Com IVA: %.2f€\n", produto.calcularValorComIVA());
            System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t   --------------------------");
        }

        System.out.println("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tTotais da Fatura:");
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tTotal Sem IVA: %.2f€\n", calcularTotalSemIVA());
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tTotal do IVA: %.2f€\n", calcularTotalIVA());
        System.out.printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tTotal Com IVA: %.2f€\n", calcularTotalComIVA());
        System.out.println("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t--------------------------");
        System.out.println("\n");

    }

    /**
     * Retorna uma representação detalhada da fatura, incluindo os produtos e valores totais.
     *
     * @return Uma string contendo os detalhes da fatura.
     */
    @Override
    public String toString() {
        String produtosDetalhes = "";
        for (Produto produto : produtos) {
            produtosDetalhes += "  - " + produto.getNome() + ", Quantidade: " + produto.getQuantidade() + ", Preço Unitário: " + produto.getValorUnitario() + "€\n";
        }

        return "Fatura Nº: " + nFatura + "\n" +
                "Cliente: " + cliente.getNome() + " (" + cliente.getnContribuinte() + ")\n" +
                "Data: " + data + "\n" +
                "Produtos:\n" + produtosDetalhes +
                "Total sem IVA: " + calcularTotalSemIVA() + "€\n" +
                "Total com IVA: " + calcularTotalComIVA() + "€\n";
    }

}
