import java.io.Serializable;

/**
 * Representa um cliente com informações sobre nome, número de contribuinte e localização.
 * Implementa a interface Serializable para que os objetos dessa classe possam ser serializados.
 */
public class Cliente implements Serializable {
    protected String nome;
    protected String nContribuinte;
    protected String localizacao;

    /**
     * Construtor da classe Cliente.
     *
     * @param nome Nome do cliente.
     * @param nContribuinte Número de contribuinte do cliente.
     * @param localizacao Localização do cliente ("Continente", "Madeira" ou "Açores").
     */
    public Cliente( String nome, String nContribuinte, String localizacao) {
        this.nome = nome;
        this.nContribuinte = nContribuinte;
        this.localizacao = localizacao; // "Continente", "Madeira" ou "Açores"
    }

    /**
     * Retorna o nome do cliente.
     *
     * @return Nome do cliente.
     */
    public String getNome() {
        return nome;
    }

    /**
     * Retorna o número de contribuinte do cliente.
     *
     * @return Número de contribuinte do cliente.
     */
    public String getnContribuinte() {
        return nContribuinte;
    }

    /**
     * Retorna a localização do cliente.
     *
     * @return Localização do cliente.
     */
    public String getLocalizacao() {
        return localizacao;
    }

    /**
     * Retorna uma representação do cliente.
     *
     * @return Uma string contendo o nome, número de contribuinte e localização do cliente.
     */
    @Override
    public String toString() {
        return "Nome: "+nome+", Número de Contribuinte: "+nContribuinte+", Localização: "+localizacao;
    }
}
