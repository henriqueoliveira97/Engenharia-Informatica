import java.io.Serializable;

/**
 * Classe abstrata que representa um produto genérico.
 * Esta classe implementa Serializable para permitir que os objetos sejam salvos em ficheiros.
 */
public abstract class Produto implements Serializable {
    protected String nome;
    protected String codigo;
    protected String descricao;
    protected int quantidade;
    protected double valorUnitario;
    protected double taxaIVA;
    protected String prodBio;

    /**
     * Construtor para criar uma instância de Produto.
     *
     * @param nome Nome do produto.
     * @param codigo Código único do produto.
     * @param descricao Descrição detalhada do produto.
     * @param quantidade Quantidade disponível do produto.
     * @param valorUnitario Valor unitário do produto.
     * @param taxaIVA Taxa de IVA aplicada ao produto.
     * @param prodBio Indicador se o produto é biológico.
     */
    Produto(String nome, String codigo, String descricao, int quantidade, double valorUnitario, double taxaIVA, String prodBio) {
        this.nome = nome;
        this.codigo = codigo;
        this.descricao = descricao;
        this.quantidade = quantidade;
        this.valorUnitario = valorUnitario;
        this.taxaIVA = 0;
        this.prodBio = (prodBio != null) ? prodBio : "não especificado"; // Evita valores nulos
    }

    /**
     * Obtém o nome do produto.
     *
     * @return Nome do produto.
     */
    public String getNome() {
        return nome;
    }

    /**
     * Obtém o código do produto.
     *
     * @return Código do produto.
     */
    public String getCodigo() {
        return codigo;
    }

    /**
     * Obtém a descrição do produto.
     *
     * @return Descrição do produto.
     */
    public String getDescricao() {
        return descricao;
    }

    /**
     * Obtém a quantidade disponível do produto.
     *
     * @return Quantidade disponível.
     */
    public int getQuantidade() {
        return quantidade;
    }

    /**
     * Define a quantidade disponível do produto.
     *
     * @param quantidade Nova quantidade disponível.
     */
    public void setQuantidade(int quantidade) {
        this.quantidade = quantidade;
    }

    /**
     * Método abstrato para definir a taxa de IVA com base na localização.
     *
     * @param localizacao Localização do cliente.
     */
    public abstract void defTaxa(String localizacao);

    /**
     * Obtém o valor unitário do produto.
     *
     * @return Valor unitário.
     */
    public double getValorUnitario() {
        return valorUnitario;
    }

    /**
     * Calcula o valor total sem IVA com base na quantidade e no valor unitário.
     *
     * @return Valor total sem IVA.
     */
    public double calcularValorSemIVA() {
        return valorUnitario * quantidade;
    }

    /**
     * Obtém a taxa de IVA aplicada ao produto.
     *
     * @return Taxa de IVA.
     */
    public double getTaxaIVA() {
        return taxaIVA;
    }

    /**
     * Calcula o valor total de IVA aplicável ao produto.
     * Leva em consideração se o produto é biológico para aplicar um desconto no IVA.
     *
     * @return Valor total de IVA.
     */
    public double calcularValorIVA() {
        if (prodBio == null || prodBio.equalsIgnoreCase("não especificado")) {
            return calcularValorSemIVA() * (taxaIVA / 100); // Valor padrão caso não seja especificado
        }

        if (prodBio.equalsIgnoreCase("sim")) {
            return (calcularValorSemIVA() * (taxaIVA / 100)) * 0.9;
        }
        return calcularValorSemIVA() * (taxaIVA / 100);
    }

    /**
     * Calcula o valor total com IVA incluído.
     *
     * @return Valor total com IVA.
     */
    public double calcularValorComIVA() {
        return calcularValorSemIVA() + calcularValorIVA();
    }

    /**
     * Retorna uma representação textual do produto.
     *
     * @return String contendo os detalhes do produto.
     */
    @Override
    public String toString() {
        return nome + "|" + codigo + "|" + descricao + "|" + quantidade + "|" +
                (double)valorUnitario + "|" + (int) taxaIVA + "|" + prodBio;
    }
}
