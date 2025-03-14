import java.io.Serializable;

/**
 * Representa um produto de farmácia, que herda da classe Produto.
 * Implementa a interface Serializable para permitir a serialização de objetos dessa classe.
 */
public class ProdFarmacia extends Produto implements Serializable {
    private boolean prescricao;
    private String medico;
    private String categoriaF;

    /**
     * Construtor da classe ProdFarmacia.
     *
     * @param nome Nome do produto.
     * @param codigo Código único do produto.
     * @param descricao Descrição detalhada do produto.
     * @param quantidade Quantidade disponível do produto.
     * @param valorUnitario Valor unitário do produto.
     * @param taxaIVA Taxa de IVA aplicada ao produto.
     * @param prodBio Indicador de se o produto é biológico.
     * @param prescricao Indica se o produto requer prescrição médica.
     * @param medico Nome do médico associado ao produto, se aplicável.
     * @param categoriaF Categoria do produto de farmácia.
     */
    public ProdFarmacia(String nome, String codigo, String descricao, int quantidade, double valorUnitario,double taxaIVA, String prodBio, boolean prescricao, String medico, String categoriaF) {
        super(nome, codigo, descricao, quantidade, valorUnitario,taxaIVA, prodBio);
        this.prescricao = prescricao;
        this.medico =medico;
        this.categoriaF=categoriaF;
    }
    /**
     * Define a taxa de IVA com base na localização do cliente e nas características do produto.
     *
     * @param localizacao Localização do cliente (ex.: "Portugal Continental", "Madeira", "Açores").
     */
    public void defTaxa(String localizacao){

        double taxaCalculada = 0.0;
        if(prescricao){
            switch (localizacao.toLowerCase()) {
                case "portugal continental":
                    taxaCalculada = 6.0;
                    break;
                case "madeira":
                    taxaCalculada = 5.0;
                    break;
                case "açores":
                    taxaCalculada = 4.0;
                    break;
                default:
                    System.out.println("Localização desconhecida.");
                    break;
            }
        }else{
            switch (localizacao.toLowerCase()) {
                case "portugal continental", "açores", "madeira":
                    taxaCalculada = 23.0;
                    if(categoriaF.equalsIgnoreCase("animais")){
                        taxaCalculada-=1;
                    }
                    break;
                default:
                    System.out.println("Localização desconhecida.");
                    break;
            }
        }

        this.taxaIVA = taxaCalculada;
    }

    /**
     * Retorna uma representação detalhada do produto de farmácia.
     *
     * @return String contendo os detalhes do produto, incluindo a prescrição e a categoria.
     */
    @Override
    public String toString() {
        return super.toString() + "|" + (prescricao ? medico: "Sem Prescrição") + "|" + categoriaF;
    }

}
