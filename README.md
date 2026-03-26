# ImageProcessor

Aplicação para processamento e análise de imagens com interface gráfica. Desenvolvido por
- Francisco Losada Totaro - 10364673
- Pedro Henrique Lanfredi Moreiras - 10441998

## Funcionalidades

- Carregamento e exibição de imagens em múltiplos formatos
- Conversão para escala de cinza
- Cálculo automático de histogramas
- Análise de propriedades da imagem:
  - Média de intensidade luminosa
  - Classificação de brilho
  - Desvio padrão de intensidade
  - Classificação de contraste
- Interface gráfica interativa baseada em SDL3

## Compilação

```bash
make
```

## Execução

```bash
./image_processor <caminho_da_imagem>
```

## Implementação Técnica

### Carregamento de Imagens
Utiliza a biblioteca SDL_image para carregar imagens em múltiplos formatos (PNG, JPEG, BMP, etc.). A imagem é armazenada em formato RGB original e convertida para escala de cinza para análise.

### Conversão para Escala de Cinza
Cada pixel RGB é convertido para um valor de intensidade de 8 bits (0-255) utilizando a fórmula 𝑌 = 0.2125 ∗ 𝑅 + 0.7154 ∗ 𝐺 + 0.0721 ∗ 𝐵. Os pixels em escala de cinza são armazenados em um array contíguo para otimizar acesso e cálculos posteriores.

### Cálculo de Histograma
O histograma é calculado iterando sobre todos os pixels e registrando a frequência de cada intensidade (0-255). A partir das frequências, calcula-se:
- Média: média ponderada das intensidades
- Desvio padrão: medida de dispersão das intensidades
- Brilho: classificado em clara (≥170), média (85-170) ou escura (<85)
- Contraste: classificado em alto (≥80), médio (30-80) ou baixo (<30)

### Interface Gráfica
Desenvolvida com SDL3 e SDL_ttf para renderização de texto. A GUI exibe:
- Visualização da imagem original
- Gráfico do histograma com frequências para cada intensidade
- Informações estatísticas e classificações
- Botões para interação com usuário

## Estrutura do Projeto
- `src/` - Código-fonte
- `include/` - Arquivos de cabeçalho
- `build/` - Arquivos compilados (gerado pelo make)
