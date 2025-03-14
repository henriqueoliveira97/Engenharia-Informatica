diretorio = 'C:\Users\Luis Oliveira\Desktop\ATD\Projeto1\data\53\';

% !IMAGENS NO PROJETO!

amplitude_maxima=zeros(10,50);
amplitude_minima=zeros(10,50);
desvio_padrao=zeros(10,50);
razao_amplitude=zeros(10,50);
energia_total=zeros(10,50);
energia_total1 = zeros(10);
energia_intervalos=zeros(10,50); % 10 linhas e 50 colunas (digitos nas linhas e instancias na coluna)

intervalo_maior_energia = zeros(10, 50);
limite_energia_silencio = 0.10;
sobreposicao = 0.75;

for i=0:9

    instancia = randi([1, 50]);
    escolha= strcat(diretorio,num2str(i),'_53_',num2str(instancia-1),'.wav');
    [audio_data, Fs]= audioread(escolha);
    
    ampMax = max(audio_data);
    ampMin = (min(abs(audio_data)));
    energia_total1(i + 1) = sum(audio_data.^2);

    janela_size = 1024;
    overlap = 0.5;
    overlap_size = round(overlap * janela_size);
    num_janelas = floor((length(audio_data) - overlap_size) / (janela_size - overlap_size));
    energia_janela = zeros(num_janelas, 1);
    for k = 1:num_janelas
        start_idx = (k - 1) * (janela_size - overlap_size) + 1;
        end_idx = start_idx + janela_size - 1;
        janela = audio_data(start_idx:end_idx);
        energia_janela(k) = sum(janela.^2);
    end

    first_index = find(abs(energia_janela) > (0.05 * energia_total1(i+1)), 1, 'first');
    start_index_audio_data = (first_index - 1) * (janela_size - overlap_size) + 1;
    audio_data = audio_data(start_index_audio_data:end);

    audio_data = audio_data ./ max(abs(audio_data));
    audio_data(length(audio_data)+1:48000) = [0];

    t = (0:length(audio_data)-1) / Fs;
    %figure(1)
    %subplot(5,2,i+1)
    %plot(t,audio_data);
    %title(num2str(i));
    %xlabel('Tempo (s)');
    %ylabel('Amplitude');
   
    
    for j = 0:49
        escolha= strcat(diretorio,num2str(i),'_53_',num2str(j),'.wav');
        [audio_data, Fs]= audioread(escolha);

        energia_total(i+1,j+1)=sum(audio_data.^2);
        fprintf('Total de Energia antes: %.4f\n', energia_total(i+1, j+1));

        janela_size = 1024;
        overlap = 0.5;
        overlap_size = round(overlap * janela_size);
        num_janelas = floor((length(audio_data) - overlap_size) / (janela_size - overlap_size));
        energia_janela = zeros(num_janelas, 1);
        for k = 1:num_janelas
            start_idx = (k - 1) * (janela_size - overlap_size) + 1;
            end_idx = start_idx + janela_size - 1;
            janela = audio_data(start_idx:end_idx);
            energia_janela(k) = sum(janela.^2);
        end

        first_index = find(abs(energia_janela) > (0.05 * energia_total(i+1, j+1)), 1, 'first');
        start_index_audio_data = (first_index - 1) * (janela_size - overlap_size) + 1;
        audio_data = audio_data(start_index_audio_data:end);

        audio_data = audio_data ./ max(abs(audio_data));
        audio_data(length(audio_data)+1:48000) = [0];
        t = (0:length(audio_data)-1) / Fs;
        
        energia_total(i+1,j+1)=sum(audio_data.^2);
        amplitude_maxima(i+1,j+1) = ampMax;
        amplitude_minima(i+1,j+1)= ampMin;
        desvio_padrao(i+1,j+1)=std(audio_data);   
        razao_amplitude(i+1,j+1)= amplitude_maxima(i+1,j+1)/amplitude_minima(i+1,j+1);
        
        intervalo_tempo = 0.1;  
        num_intervalos = floor(length(audio_data) / (Fs * intervalo_tempo));
        energia_por_intervalo = zeros(1, num_intervalos);
        for k = 1:num_intervalos
            indice_inicio = round((k - 1) * Fs * intervalo_tempo) +1;
            indice_fim = round(k * Fs * intervalo_tempo);
            energia_por_intervalo(k) = sum(audio_data(indice_inicio:indice_fim).^2);
        end
        [energia_maxima, indice_maximo] = max(energia_por_intervalo); % Encontrando o índice do intervalo com a maior energia
        energia_intervalos(i + 1, j + 1) = energia_maxima;
        intervalo_maior_energia(i + 1, j + 1) = indice_maximo;
        
        fprintf('Dígito: %d, Instância: %d\n', i, j);
        fprintf('Total de Energia: %.4f\n', energia_total(i+1, j+1));
        fprintf('Energia Máxima por Intervalo: %.4f\n', energia_maxima);
        fprintf('Índice do Intervalo com Maior Energia: %d\n', indice_maximo);
        fprintf('Amplitude Máxima: %.4f\n', amplitude_maxima(i+1, j+1));
        fprintf('Desvio Padrão: %.4f\n\n', desvio_padrao(i+1, j+1));
    end
end

% Boxplot da energia total
figure
subplot(3, 2, 1);
boxplot(rmoutliers(energia_total'));
title('Energia Total por Dígito');
xlabel('Dígito');
ylabel('Energia Total');

% Boxplot da Energia por intervalos
subplot(3, 2, 2);
boxplot(rmoutliers(energia_intervalos'));
title('Energia Máxima por Intervalo por Dígito');
xlabel('Dígito');
ylabel('Energia Máxima');

% Boxplot do Intervalo com Maior Energia
subplot(3, 2, 3);
boxplot(intervalo_maior_energia');
title('Intervalo com Maior Energia por Dígito');
xlabel('Dígito');
ylabel('Índice do Intervalo com Maior Energia');

% Boxplot da amplitude máxima
subplot(3, 2, 4);
boxplot(amplitude_maxima');
title('Amplitude Máxima por Dígito');
xlabel('Dígito');
ylabel('Amplitude Máxima');

% Boxplot do desvio padrão
subplot(3, 2, 5);
boxplot(rmoutliers(desvio_padrao'));
title('Desvio Padrão por Dígito');
xlabel('Dígito');
ylabel('Desvio Padrão');

%% meta 2
diretorio = 'C:\Users\Luis Oliveira\Desktop\ATD\Projeto1\data\53\';
 % Inicializar matrizes para armazenar as características espectrais
    num_instancias = 50;
    num_digitos = 10;
    
    maximos_espectrais_idx = zeros(num_instancias, num_digitos);
    maximos_espectrais_amp = zeros(num_instancias, num_digitos);
    medias_espectrais = zeros(num_instancias, num_digitos);
    spectral_edge_frequencies = zeros(num_instancias, num_digitos);
    energia_total = zeros(num_digitos, num_instancias);
    energia_espectral = zeros(num_instancias, num_digitos);
    
    % Lista de nomes das janelas
    nomes_janelas = {'Normal','Hamming', 'Hann', 'Blackman'};
    %janelas = {@hamming, @hann, @blackman};
    

for j = 1:length(janelas) +1 % Loop através das janelas
    %figure('Name', nomes_janelas{j}); % Criar uma nova figura para cada janela
    for i = 0:9
        % Arrays para armazenar os espectros de amplitude de todas as instâncias
        espectros_normalizados = zeros(50, 48000/2);

        for instancia = 1:50

            escolha = strcat(diretorio, num2str(i), '_53_', num2str(instancia-1), '.wav');
            [audio_data, Fs] = audioread(escolha);
            
            ampMax = max(audio_data);
            ampMin = (min(abs(audio_data)));

            energia_total(i+1,instancia+1)=sum(audio_data.^2);

            janela_size = 1024; %tamanho da janela
            overlap = 0.5;
            overlap_size = round(overlap * janela_size); %tamanho do overlap
            num_janelas = floor((length(audio_data) - overlap_size) / (janela_size - overlap_size));
            energia_janela = zeros(num_janelas, 1); %energia por janelas
            for k = 1:num_janelas
                start_idx = (k - 1) * (janela_size - overlap_size) + 1; 
                end_idx = start_idx + janela_size - 1;
                janela = audio_data(start_idx:end_idx);
                energia_janela(k) = sum(janela.^2);
            end

            first_index = find(abs(energia_janela) > (0.05 * energia_total(i+1, j+1)), 1, 'first');
            start_index_audio_data = (first_index - 1) * (janela_size - overlap_size) + 1;
            audio_data = audio_data(start_index_audio_data:end);

            audio_data = audio_data ./ max(abs(audio_data));
            audio_data(length(audio_data)+1:48000) = [0];
            t = (0:length(audio_data)-1) / Fs;

            %dft
            
            if j ~= 1
                % Aplicar a janela
                janela = janelas{j-1}(length(audio_data));
                audio_data = audio_data .* janela;
            end

            dft_audio = fft(audio_data);
            dft_audio(abs(dft_audio) < 0.001) = 0;

            num_amostras = length(audio_data);
            %mudar a freq fotos aula (Ws/2:ws/N:ws/2-ws/N
            frequencias = Fs*(0:(num_amostras/2))/num_amostras;
            frequencias_positivas = frequencias(1:num_amostras/2);
            
            modulo_dft = abs(dft_audio(1:num_amostras/2)); %modulo da transformada discreta
            espectro_normalizado = modulo_dft / num_amostras;
            espectros_normalizados(instancia, :) = espectro_normalizado';

            % Calcular as características espectrais
            % 1. Máximos espectrais
            [max_amp, idx_max_amp] = max(espectro_normalizado);
            maximos_espectrais_idx(instancia, i+1) = idx_max_amp; % Armazenar a indice do máximo espectral
            maximos_espectrais_amp(instancia, i+1) = max_amp; % Armazenar a amplitude do máximo espectral

            % 2. Médias espectrais
            %medias_espectrais(instancia, i+1) = mean(sum(frequencias_positivas'.*espectro_normalizado)./sum(espectros_normalizados));
            medias_espectrais(instancia, i+1) = mean(espectro_normalizado);
            %medias_espectrais(instancia, i+1) = mean(abs(espectro_normalizado).^2);

            % 3. Spectral edge frequency
            percentual_energia = 0.90; % 90% da energia
            %cumsum - soma acumulada
            idx_spectral_edge = find(cumsum(espectro_normalizado) >= percentual_energia * sum(espectro_normalizado), 1,'first');
            % idx da primeira freq em que a soma acumulada do espetro é > que 90% da energia total
            spectral_edge_frequencies(instancia, i+1) = frequencias_positivas(idx_spectral_edge);

            % 4. Energia espectral (energia total)
            energia_espectral(instancia, i+1) = sum(abs(espectro_normalizado).^2);

        end

        % Calcular os quartis
        primeiro_quartil = quantile(espectros_normalizados, 0.25);
        mediano =median(espectros_normalizados);
        terceiro_quartil = quantile(espectros_normalizados, 0.75);

        %limitar para melhor visualização
        frequencias_positivas_lim = frequencias_positivas(frequencias_positivas <= 8000);

        %subplot(5, 2, i+1);
        %plot(frequencias_positivas_lim, terceiro_quartil(1:length(frequencias_positivas_lim)), 'g','LineWidth',0.5);
        %hold on;
        %plot(frequencias_positivas_lim, mediano(1:length(frequencias_positivas_lim)), 'b','LineWidth',0.5);
        %plot(frequencias_positivas_lim, primeiro_quartil(1:length(frequencias_positivas_lim)), 'r','LineWidth',0.5);
        %hold off;
        %xlabel('Frequência');
        %ylabel('Amplitude');
        %title(['Dígito ' num2str(i)]);
        %legend('Mediana', 'Q25', 'Q75');
        %grid on;

    end
    if j == 1
        figure
        % Máximos espectrais (amp)
        subplot(2, 3, 1);
        boxplot(rmoutliers(maximos_espectrais_amp), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
        xlabel('Dígito');
        ylabel('amplitude');
        title('Máximos Espectrais - amp');

        % Máximos espectrais (indice) %Pôr a freuqencia
        subplot(2, 3, 2);
        boxplot(rmoutliers(maximos_espectrais_idx), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
        xlabel('Dígito');
        ylabel('indice');
        title('Máximos Espectrais - idx');
        figure
        % Médias espectrais
        subplot(2, 3, 3);
        boxplot(rmoutliers(medias_espectrais), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
        xlabel('Dígito');
        ylabel('Média');
        title('Médias Espectrais');

        % Spectral edge frequency
        subplot(2, 3, 4);
        boxplot(rmoutliers(spectral_edge_frequencies), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
        xlabel('Dígito');
        ylabel('Frequência (Hz)');
        title('Spectral Edge Frequency');

        % Energia espectral
        subplot(2, 3, 5);
        boxplot(rmoutliers(energia_espectral), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
        xlabel('Dígito');
        ylabel('Energia');
        title('Energia Espectral');
    end
end


%% Meta 3
diretorio = 'C:\Users\Luis Oliveira\Desktop\ATD\Projeto1\data\53\';

num_digits = 10; % Número de dígitos (0 a 9)
num_samples = 50; % Número de amostras por dígito

% Inicialização das estruturas para armazenar características
frequencias_picos = zeros(num_samples,num_digits);
picos = zeros(num_samples,num_digits);
numero_picos = zeros(num_samples, num_digits);
energia_distribuida=zeros(num_samples,num_digits);
contraste_picos=zeros(num_samples,num_digits);
first_freq_dominante = zeros(num_samples,num_digits);
p_media = zeros(num_samples, num_digits);
%mean_power = zeros(num_samples, num_digits);
peak_power = zeros(num_samples, num_digits);
power_std = zeros(num_samples, num_digits);
spectral_rolloff = zeros(num_samples, num_digits);

for j = 0:num_digits-1
    for k = 1:num_samples
        escolha= strcat(diretorio,num2str(j),'_53_',num2str(k-1),'.wav');

        [audio_data,fs] = audioread(escolha);

        N = numel(audio_data);

        % largura da janela de análise em s
        Tframe = 0.011;
        % sobreposiçao das janelas em s
        Toverlap= 0.005;
        % número de amostras na janela
        Nframe= round(Tframe*fs);
        % número de amostras sobrepostas na janela
        Noverlap= round(Toverlap*fs);

        % janela de hamming
        h= hamming(Nframe);

        if mod(Nframe, 2)==0
            f_frame= -fs/2:fs/Nframe:fs/2-fs/Nframe;
        else 
            f_frame = -fs/2+fs/(2*Nframe):fs/Nframe:fs/2-fs/(2*Nframe);
        end
        
        
        nframes = 1;
        p_total = 0;

        step_size = Nframe - Noverlap; % Tamanho do passo entre segmentos
        num_segments = floor((N - Nframe) / step_size) + 1; % Número total de segmentos

        % Inicialização de variáveis temporárias para armazenar características
        %mean_power_temp = zeros(1, num_segments);
        peak_power_temp = zeros(1, num_segments);
        power_std_temp = zeros(1, num_segments);
        spectral_rolloff_temp = zeros(1, num_segments);
        frequencias_picos_temp = zeros(1, num_segments);
        energia_distribuida_temp = zeros(1, num_segments);
        contraste_picos_temp = zeros(1, num_segments);
        picos_temp = zeros(1, num_segments);
        numero_picos_temp = zeros(1, num_segments);

        %itera pelos indices iniciais
        for ii = 1:Nframe-Noverlap:N-Nframe
            
            % aplicar a janela (Hamming) ao sinal do tempo - considear o vetor de dados dentro da janela aplicando a janela de Hamming
            x_frame= audio_data(ii:ii+Nframe-1).* h;
            
            % obter a magnitude da fft do sinal
            m_X_frame= abs(fftshift(fft(x_frame)));
            
            % obter o máximo da magnitude do sinal
            m_X_frame_max= max(m_X_frame);

            [s,f,t,p] = spectrogram(audio_data,x_frame,Noverlap,[],fs);

            %picos na magnitude do sinal
            [pks, locs] = findpeaks(m_X_frame, 'MinPeakHeight', max(m_X_frame) * 0.8);
            
            picos_temp(nframes) = mean(pks); %picos de magnitude
            numero_picos_temp(nframes) = numel(pks);

            freq_peaks = f(locs); % frequencias do dos picos
            frequencias_picos_temp(nframes) = mean(freq_peaks);
            
            %distribuição de energia (somatório das potências)
            energia_distribuida_temp(nframes) = sum(p(:));

            p_frame = sum(m_X_frame.^2)/Nframe;
            p_total= p_total + p_frame;
            
            s1 = abs(s(:, 1));
            [aux, locs] = max(s1);
            if ii==1
                first_freq_dominante(k, j+1) = f(locs);
            end

            % contraste entre picos e valores abaixo
            media_picos = mean(pks);
            media_valores_baixos = mean(p(p < max(p(:)) * 0.8)); % abaixo do pico
            contraste_picos_temp(ii) = media_picos / media_valores_baixos;

            %potencia media
            %mean_power_temp(nframes) = mean(p(:));

            %pico de potência
            peak_power_temp(nframes) = max(p(:));

            % desvio padrão da potência 
            power_std_temp(nframes) = std(p(:));

            %roll-off espectral 
            %frequência abaixo na qual uma certa porcentagem (geralmente 85%)
            %da energia total do espectro é acumulada
            %spectral_rolloff_temp(nframes) = find(cumsum(p( :)) >= 0.85 * sum(p( :)), 1);

            rolloff_idx = find(cumsum(p(:, nframes)) >= 0.85 * sum(p(:, nframes)), 1);
            
            % Convert the index to frequency (Hz)
            if ~isempty(rolloff_idx)
                spectral_rolloff_temp(nframes) = f_frame(rolloff_idx);
            else
                spectral_rolloff_temp(nframes) = NaN; % Set NaN if no index found
            end
            
            nframes = nframes + 1;
     
        end 

        %agrega as características temporárias
        %mean_power(k, j+1) = mean(mean_power_temp);
        peak_power(k, j+1) = max(peak_power_temp);
        power_std(k, j+1) = mean(power_std_temp);
        spectral_rolloff(k, j+1) = mean(spectral_rolloff_temp, 'omitnan');

        frequencias_picos(k, j+1) = mean(frequencias_picos_temp);
        energia_distribuida(k, j+1) = mean(energia_distribuida_temp);
        contraste_picos(k, j+1) = mean(contraste_picos_temp);
        picos(k, j+1) = mean(picos_temp);
        numero_picos(k, j+1) = mean(numero_picos_temp);

        p_media(k, j+1) = p_total / (N / (Nframe-Noverlap));
        
    end
    
    %figure(2);
    %subplot(5, 2, j+1)
    %hold on
    %spectrogram(x,h, Noverlap, [], fs, "yaxis");
    
    
end

%Boxplots
figure;
subplot(3, 3, 1);
boxplot(rmoutliers(picos), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Amplitude dos Picos por Dígito');
xlabel('Dígito');
ylabel('Amplitude Média');

subplot(3, 3, 2);
boxplot(rmoutliers(energia_distribuida), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Energia Distribuída por Dígito');
xlabel('Dígito');
ylabel('Energiaa');

subplot(3, 3, 3);
boxplot(rmoutliers(contraste_picos), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Contraste dos Picos por Dígito');
xlabel('Dígito');
ylabel('Contraste');

subplot(3, 3, 4);
boxplot(rmoutliers(p_media), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Potencia Media');
xlabel('Dígito');
ylabel('Potencia');

subplot(3, 3, 5);
boxplot(rmoutliers(peak_power), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Pico de Potência');
xlabel('Dígito');
ylabel('Potência');

subplot(3, 3, 6);
boxplot(rmoutliers(power_std), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Desvio Padrão da Potênciao');
xlabel('Dígito');
ylabel('Potência');

subplot(3, 3, 7);
boxplot(rmoutliers(spectral_rolloff), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Roll-Off Espectral por digito');
xlabel('Dígito');
ylabel('Roll-Off Espectral');

subplot(3, 3, 8);
boxplot(rmoutliers(numero_picos), 'Labels', {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
title('Número de Picos por Dígito');
xlabel('Dígito');
ylabel('Número de Picos');

          

%% Meta4
num_acertos = 0;         
for k = 1:10
    for j = 1:50
        if picos(j,k) >= 0.21521 && picos(j,k) <= 0.4201 ...
                && contraste_picos(j, k) >= 2112857.9379 && contraste_picos(j, k) <= 5793919.9824 ...
                && spectral_rolloff(j, k) >= 516.0846 && spectral_rolloff(j, k) <= 2566.226 ...
                && medias_espectrais(j,k) >= 8.7876e-5 && medias_espectrais(j,k) <= 0.00015304 ...
                && spectral_edge_frequencies(j,k) >= 3453 && spectral_edge_frequencies(j,k) <= 7743 ...
                && energia_espectral(j, k) >= 0.0072761 && energia_espectral(j, k) <= 0.023175 ...
                && energia_total_meta1(k, j) >= 619.2935 && energia_total_meta1(k, j) <= 2033.0844 ...
                && energia_intervalos(k, j) >= 368.9617 && energia_intervalos(k, j) <= 859.6512 ...
                && desvio_padrao(k, j) >= 0.11359 && desvio_padrao(k, j) <= 0.2058
            digit = 0;
        
        % Dígito 1
        elseif picos(j,k) >= 0.20762 && picos(j,k) <= 0.35865 ...
                && contraste_picos(j, k) >= 3606845.8042 && contraste_picos(j, k) <= 6987646.5836 ...
                && spectral_rolloff(j,k) >= 286.5902 && spectral_rolloff(j, k) <= 1094.5312 ...
                && medias_espectrais(j,k) >= 8.0401e-5 && medias_espectrais(j,k) <= 0.00013033 ...
                && spectral_edge_frequencies(j,k) >= 3190 && spectral_edge_frequencies(j,k) <= 7488 ...
                && energia_espectral(j, k) >= 0.0050788 && energia_espectral(j, k) <= 0.015542 ...
                && energia_total_meta1(k, j) >= 544.0432 && energia_total_meta1(k, j) <= 1578.566 ...
                && energia_intervalos(k, j) >= 268.6867 && energia_intervalos(k, j) <= 698.115 ...
                && desvio_padrao(k, j) >= 0.10646 && desvio_padrao(k, j) <= 0.18135
            digit = 1;
    
        % Dígito 2
        elseif picos(j,k) >= 0.22699 && picos(j,k) <= 0.46057 ...
                && contraste_picos(j,k) >= 3205881.6649 && contraste_picos(j, k) <= 6088064.3642 ...
                && spectral_rolloff(j, k) >= 560.5736 && spectral_rolloff(j, k) <= 1654.7379 ...
                && medias_espectrais(j,k) >= 6.6847e-5 && medias_espectrais(j,k) <= 9.0253e-5 ...
                && spectral_edge_frequencies(j,k) >= 0 && spectral_edge_frequencies(j,k) <= 8596 ...
                && energia_espectral(j, k) >= 0.0054034 && energia_espectral(j, k) <= 0.01719 ...
                && energia_total_meta1(k, j) >= 478.6256 && energia_total_meta1(k, j) <= 1650.2143 ...
                && energia_intervalos(k, j) >= 310.1278 && energia_intervalos(k, j) <= 960.8514 ...
                && desvio_padrao(k, j) >= 0.099855 && desvio_padrao(k, j) <= 0.19438
            digit = 2;
    
        % Dígito 3
        elseif picos(j,k) >= 0.22993 && picos(j,k) <= 0.50257 ...
                && contraste_picos(j,k) >= 2123998.3487 && contraste_picos(j,k) <= 4832922.6638 ...
                && spectral_rolloff(j,k) >= 417.324 && spectral_rolloff(j,k) <= 3179.4355 ...
                && medias_espectrais(j,k) >= 7.7986e-5 && medias_espectrais(j,k) <= 0.00010933 ...
                && spectral_edge_frequencies(j,k) >= 3128 && spectral_edge_frequencies(j,k) <= 4172 ...
                && energia_espectral(j,k) >= 0.004071 && energia_espectral(j,k) <= 0.017023 ...
                && energia_total_meta1(k, j) >= 390.814 && energia_total_meta1(k, j) <= 1613.6896 ...
                && energia_intervalos(k, j) >= 359.0132 && energia_intervalos(k, j) <= 896.9834 ...
                && desvio_padrao(k, j) >= 0.090233 && desvio_padrao(k, j) <= 0.18676
            digit = 3;
    
        elseif picos(j,k) >= 0.25108 && picos(j,k) <= 0.38907 ...
                && contraste_picos(j,k) >= 2646092.5547 && contraste_picos(j,k) <= 5094218.7014 ...
                && spectral_rolloff(j,k) >= 229.4695 && spectral_rolloff(j,k) <= 3933.7871 ...
                && medias_espectrais(j,k) >= 8.3539e-5 && medias_espectrais(j,k) <= 0.00012004 ...
                && spectral_edge_frequencies(j,k) >= 1816 && spectral_edge_frequencies(j,k) <= 3701 ...
                && energia_espectral(j,k) >= 0.0069806 && energia_espectral(j,k) <= 0.019240 ...
                && energia_total_meta1(k, j) >= 670.1364 && energia_total_meta1(k, j) <= 1896.5176 ...
                && energia_intervalos(k, j) >= 362.1811 && energia_intervalos(k, j) <= 643.0379 ...
                && desvio_padrao(k, j) >= 0.11816 && desvio_padrao(k, j) <= 0.19878
            digit = 4;
    
        % Dígito 5
        elseif picos(j,k) >= 0.1738 && picos(j,k) <= 0.3682 ...
                && contraste_picos(j,k) >= 2915567.3727 && contraste_picos(j,k) <= 7215319.6484 ...
                && spectral_rolloff(j,k) >= 417.239 && spectral_rolloff(j,k) <= 5663.1098 ...
                && medias_espectrais(j,k) >= 6.9452e-5 && medias_espectrais(j,k) <= 0.00012936 ... 
                && spectral_edge_frequencies(j,k) >= 3526 && spectral_edge_frequencies(j,k) <= 9524 ...
                && energia_espectral(j,k) >= 0.0018307 && energia_espectral(j,k) <= 0.0087857 ...
                && energia_total_meta1(k, j) >= 78.9607 && energia_total_meta1(k, j) <= 910.459 ...
                && energia_intervalos(k, j) >= 114.3123 && energia_intervalos(k, j) <= 383.9988 ...
                && desvio_padrao(k, j) >= 0.06057 && desvio_padrao(k, j) <= 0.14287
            digit = 5;
    
        % Dígito 6
        elseif picos(j,k) >= 0.15338 && picos(j,k) <= 0.40728 ...
                && contraste_picos(j,k) >= 3146726.1748 && contraste_picos(j,k) <= 6428115.6779 ...
                && spectral_rolloff(j,k) >= 2064.2361 && spectral_rolloff(j,k) <= 7693.1728 ...
                && medias_espectrais(j,k) >= 0.00013557 && medias_espectrais(j,k) <= 0.00027628 ...
                && spectral_edge_frequencies(j,k) >= 7142 && spectral_edge_frequencies(j,k) <= 10850 ...
                && energia_espectral(j,k) >= 0 && energia_espectral(j,k) <= 0.012964 ...
                && energia_total_meta1(k, j) >= 419.9661 && energia_total_meta1(k, j) <= 1184.0916 ...
                && energia_intervalos(k, j) >= 165.8509 && energia_intervalos(k, j) <= 571.0488 ...
                && desvio_padrao(k, j) >= 0.088073 && desvio_padrao(k, j) <= 0.18661
            digit = 6;
    
        % Dígito 7
        elseif picos(j,k) >= 0.17639 && picos(j,k) <= 0.43057 ...
                && contraste_picos(j,k) >= 2417684.1909 && contraste_picos(j,k) <= 6305121.71 ...
                && spectral_rolloff(j,k) >= 1649.8067 && spectral_rolloff(j,k) <= 4357.736 ...
                && medias_espectrais(j,k) >= 7.1765e-5 && medias_espectrais(j,k) <= 0.00015914 ... 
                && spectral_edge_frequencies(j,k) >= 4284 && spectral_edge_frequencies(j,k) <= 8977 ...
                && energia_espectral(j,k) >= 0.0018595 && energia_espectral(j,k) <= 0.0085324 ...
                && energia_total_meta1(k, j) >= 328.4375 && energia_total_meta1(k, j) <= 997.2176 ...
                && energia_intervalos(k, j) >= 172.0951 && energia_intervalos(k, j) <= 433.1471 ...
                && desvio_padrao(k, j) >= 0.08272 && desvio_padrao(k, j) <= 0.16034
            digit = 7;
    
        % Dígito 8
        elseif picos(j,k) >= 0.17536 && picos(j,k) <= 0.39709 ...
                && contraste_picos(j,k) >= 2518971.9905 && contraste_picos(j,k) <= 5495196.5402 ...
                && spectral_rolloff(j,k) >= 625.6188 && spectral_rolloff(j,k) <= 3070.1014 ...
                && medias_espectrais(j,k) >= 9.4998e-5 && medias_espectrais(j,k) <= 0.00015315 ... 
                && spectral_edge_frequencies(j,k) >= 4543 && spectral_edge_frequencies(j,k) <= 8065 ...
                && energia_espectral(j,k) >= 0.0045991 && energia_espectral(j,k) <= 0.014317 ...
                && energia_total_meta1(k, j) >= 441.5034 && energia_total_meta1(k, j) <= 1374.4368 ...
                && energia_intervalos(k, j) >= 242.8137 && energia_intervalos(k, j) <= 722.0263 ...
                && desvio_padrao(k, j) >= 0.086076 && desvio_padrao(k, j) <= 0.16922
            digit = 8;
    
        % Dígito 9
        elseif picos(j,k) >= 0.17247 && picos(j,k) <= 0.4542 ...
                && contraste_picos(j,k) >= 4363952.2604 && contraste_picos(j,k) <= 7473689.4555 ...
                && spectral_rolloff(j,k) >= 223.5938 && spectral_rolloff(j,k) <= 971.7988 ...
                && medias_espectrais(j,k) >= 0.00010837 && medias_espectrais(j,k) <= 0.00018641 ...
                && spectral_edge_frequencies(j,k) >= 5795 && spectral_edge_frequencies(j,k) <= 9191 ...
                && energia_espectral(j,k) >= 0.0092807 && energia_espectral(j,k) <= 0.025509 ...
                && energia_total_meta1(k, j) >= 840.3176 && energia_total_meta1(k, j) <= 2692.4881 ...
                && energia_intervalos(k, j) >= 262.8282 && energia_intervalos(k, j) <= 721.3258 ...
                && desvio_padrao(k, j) >= 0.13231 && desvio_padrao(k, j) <= 0.23683
            digit = 9;

    
        else
            digit = -1;  % Nenhum dígito correspondente encontrado
        
        end
        digit
        if digit == k-1
            num_acertos = num_acertos + 1
        end
    end 
    
end
num_acertos
acertos = (num_acertos/(10*50))*100
