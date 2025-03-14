import java.util.List;

/**
 * Classe responsável por realizar diversas validações na aplicação.
 * Este verificador é utilizado para validar entradas de clientes, produtos e faturas.
 */
public class Verificador {
    /**
     * Verifica se os dados fornecidos para um cliente são válidos.
     *
     * @param localizacao Localização do cliente ("Portugal Continental", "Madeira" ou "Açores").
     * @param nContribuinte Número de contribuinte do cliente.
     * @param nome Nome do cliente .
     * @param clientes Lista de clientes existente.
     * @return true se os dados forem válidos, false caso contrário.
     */
    public boolean verify(String localizacao, String nContribuinte, String nome, List<Cliente> clientes){
        if (!(localizacao.equalsIgnoreCase("Portugal Continental") || localizacao.equalsIgnoreCase("Madeira") || localizacao.equalsIgnoreCase("Açores"))) {
            return false;
        }
        if (nome == null || nome.isEmpty()) {
            return false;
        }
        char primeiraLetra = nome.charAt(0);
        if (primeiraLetra < 'A' || primeiraLetra > 'Z') {
            return false;
        }
        for (int i = 1; i < nome.length(); i++) {
            char c = nome.charAt(i);
            if (c < 'a' || c > 'z') {
                return false;
            }
        }
        if(nContribuinte.length()!=9)
            return false;
        for (int i = 1; i < nContribuinte.length(); i++) {
            char c = nContribuinte.charAt(i);
            if (c < '0' || c > '9') {
                return false;
            }
        }        for (Cliente cliente : clientes) {
            if (cliente.nContribuinte.equals(nContribuinte)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Verifica se os dados fornecidos para um produto alimentar são válidos.
     *
     * @param codigo Código do produto.
     * @param descricao Descrição do produto.
     * @param quantidade Quantidade do produto.
     * @param valorUnitario Valor unitário do produto.
     * @param taxa Tipo de taxa ("reduzida", "intermedia" ou "normal").
     * @param categoria Categoria do produto ("congelados", "enlatados" ou "vinho").
     * @param produtosA Lista de produtos alimentares existente.
     * @return true se os dados forem válidos, false caso contrário.
     */
    public boolean verifyProdA(String codigo, String descricao, int quantidade, double valorUnitario,String taxa, String categoria, List<ProdAlimentares> produtosA){
        if(descricao.equalsIgnoreCase(""))
            return false;
        if(quantidade <=0)
            return false;
        if(valorUnitario<=0)
            return false;
        if(!(taxa.equalsIgnoreCase("reduzida")|| taxa.equalsIgnoreCase("intermedia") || taxa.equalsIgnoreCase("normal")))
            return false;
        if(!(categoria.equalsIgnoreCase("congelados")|| categoria.equalsIgnoreCase("enlatados")|| categoria.equalsIgnoreCase("vinho")))
            return false;
        for(ProdAlimentares prod : produtosA){
            if (prod.getCodigo().equals(codigo)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Verifica se os dados fornecidos para um produto de farmácia são válidos.
     *
     * @param codigo Código do produto.
     * @param descricao Descrição do produto.
     * @param quantidade Quantidade do produto.
     * @param valorUnitario Valor unitário do produto.
     * @param medico Nome do médico associado (deve começar com uma letra maiúscula).
     * @param categoria Categoria do produto ("beleza", "bem-estar", "bebés", "animais" ou "outro").
     * @param produtosF Lista de produtos de farmácia existente.
     * @return true se os dados forem válidos, false caso contrário.
     */
    public boolean verifyProdF(String codigo,String descricao, int quantidade,double valorUnitario,String medico,String categoria, List<ProdFarmacia> produtosF){
        if(descricao.equalsIgnoreCase(""))
            return false;
        if(quantidade <=0)
            return false;
        if(valorUnitario<=0)
            return false;
        if (medico.charAt(0) < 'A' || medico.charAt(0) > 'Z') {
            return false;
        }
        if(!(categoria.equalsIgnoreCase("beleza")|| categoria.equalsIgnoreCase("bem-estar")|| categoria.equalsIgnoreCase("bebés")
                || categoria.equalsIgnoreCase("animais") ||categoria.equalsIgnoreCase("outro")))
            return false;
        for(ProdFarmacia prod : produtosF){
            if (prod.getCodigo().equals(codigo)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Verifica se os dados fornecidos para uma fatura são válidos.
     *
     * @param nFatura Número da fatura (deve ter exatamente 6 caracteres).
     * @param cliente Cliente associado à fatura.
     * @param data Data da fatura (deve estar no formato válido).
     * @param clientes Lista de clientes existente.
     * @param faturas Lista de faturas existente.
     * @return true se os dados forem válidos, false caso contrário.
     */
    public boolean verifyFaturas(String nFatura, Cliente cliente, String data, List<Cliente> clientes, List<Fatura> faturas){
        if(nFatura.length()!= 6){
            return false;
        }
        if (!clientes.contains(cliente)) {
            return false;
        }
        if (!VerifyData(data)) {
            return false;
        }
        for (Fatura fatura : faturas) {
            if (fatura.getnFatura().equals(nFatura)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Verifica se a data fornecida é válida.
     *
     * @param data Data no formato "dd/MM/yyyy".
     * @return true se a data for válida, false caso contrário.
     */
    private boolean VerifyData(String data){
        if (data == null || data.isEmpty()) {
            return false;
        }
        String[] dados = data.split("/");
        if (dados.length != 3) {
            return false;
        }
        int dia = Integer.parseInt(dados[0]);
        int mes = Integer.parseInt(dados[1]);
        int ano = Integer.parseInt(dados[2]);
        if (ano < 2020 || ano > 2024) {
            return false;
        }
        if (mes < 1 || mes > 12) {
            return false;
        }
        if (dia < 1 || dia > 31) {
            return false;
        }
        return true;
    }
}
