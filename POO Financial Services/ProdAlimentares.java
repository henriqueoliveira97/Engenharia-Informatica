import java.io.Serializable;

/**
 * Representa produtos alimentares, que herdam da classe Produto.
 * Implementa a interface Serializable para permitir a serialização de objetos dessa classe.
 */
public class ProdAlimentares extends Produto implements Serializable {
    private String taxa;
    private  String[] certificacao;
    private String categoria;

    /**
     * Construtor para criar uma instância de ProdAlimentares.
     *
     * @param nome Nome do produto.
     * @param codigo Código único do produto.
     * @param descricao Descrição detalhada do produto.
     * @param quantidade Quantidade disponível do produto.
     * @param valorUnitario Valor unitário do produto.
     * @param taxaIVA Taxa de IVA aplicada ao produto.
     * @param prodBio Indicador de se o produto é biológico.
     * @param taxa Tipo de taxa aplicável ao produto.
     * @param certificacao Certificações associadas ao produto.
     * @param categoria Categoria do produto.
     */
    public ProdAlimentares(String nome, String codigo, String descricao, int quantidade, double valorUnitario, double taxaIVA, String prodBio, String taxa, String[] certificacao, String categoria) {
        super(nome, codigo, descricao, quantidade, valorUnitario, taxaIVA, prodBio);
        this.taxa = taxa; // "reduzida", "intermedia", ou "normal"
        this.certificacao = certificacao;
        this.categoria = categoria; // "congelados", "enlatados" ou "vinho"
    }

    /**
     * Define a taxa de IVA com base na localização do cliente e nas características do produto.
     * Considera o tipo de taxa (reduzida, intermedia ou normal) e certificações aplicáveis.
     *
     * @param localizacao Localização do cliente (ex.: "Portugal Continental", "Madeira", "Açores").
     */
    public void defTaxa(String localizacao) {
        double taxaCalculada = 0.0;
        switch (taxa.toLowerCase()) {
            case "reduzida":
                switch (localizacao.toLowerCase()) {
                    case "portugal continental":
                        taxaCalculada = 6.0;
                        if(certificacao.length==4){
                            taxaCalculada-=1;
                        }
                        break;
                    case "madeira":
                        taxaCalculada = 5.0;
                        if(certificacao.length==4){
                            taxaCalculada-=1;
                        }
                        break;
                    case "açores":
                        taxaCalculada = 4.0;
                        if(certificacao.length==4){
                            taxaCalculada-=1;
                        }
                        break;
                    default:
                        System.out.println("Localização desconhecida.");
                        break;
                }
                break;

            case "intermedia":
                switch (localizacao.toLowerCase()) {
                    case "portugal continental":
                        taxaCalculada = 13.0;
                        if(categoria.equalsIgnoreCase("vinho")){
                            taxaCalculada+=1;
                        }
                        break;
                    case "madeira":
                        taxaCalculada = 12.0;
                        if(categoria.equalsIgnoreCase("vinho")){
                            taxaCalculada+=1;
                        }
                        break;
                    case "açores":
                        taxaCalculada = 9.0;
                        if(categoria.equalsIgnoreCase("vinho")){
                            taxaCalculada+=1;
                        }
                        break;
                    default:
                        System.out.println("Localização desconhecida.");
                        break;
                }
                break;

            case "normal":
                switch (localizacao.toLowerCase()) {
                    case "portugal continental":
                        taxaCalculada = 23.0;
                        break;
                    case "madeira":
                        taxaCalculada = 22.0;
                        break;
                    case "açores":
                        taxaCalculada = 16.0;
                        break;
                    default:
                        System.out.println("Localização desconhecida.");
                        break;
                }
                break;

            default:
                System.out.println("Tipo de taxa desconhecida.");
                break;
        }

        this.taxaIVA = taxaCalculada;
    }
    /**
     * Retorna uma representação detalhada do produto alimentar.
     *
     * @return String contendo os detalhes do produto, incluindo a categoria.
     */
    @Override
    public String toString() {
        return super.toString() + "|" + categoria;
    }


}


