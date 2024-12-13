# Documentação do Semáforo Inteligente IoT

Aluno: Lucas Magalhães Castro Rodrigues

## 1. Estados do Sistema

### 1.1 Inicialização
O sistema inicia com todos os LEDs apagados e estabelece conexão com a rede WiFi. Este é o estado inicial do sistema antes de começar o funcionamento normal.

### 1.2 Modo Noturno
Ativado quando o sensor LDR detecta baixa luminosidade (leitura ≤ 600):
- LED amarelo pisca em intervalos de 1 segundo
- LEDs verde e vermelho permanecem apagados
- Sistema permanece neste estado até que a luminosidade aumente

### 1.3 Modo Diurno (Funcionamento Normal)
Ativado quando há luminosidade adequada (leitura > 600), opera em ciclo:

#### 1.3.1 Sinal Verde (3 segundos)
- LED verde aceso
- Demais LEDs apagados
- Permite passagem livre

#### 1.3.2 Sinal Amarelo (2 segundos)
- LED amarelo aceso
- Demais LEDs apagados
- Indica atenção, preparação para parada

#### 1.3.3 Sinal Vermelho (5 segundos)
- LED vermelho aceso
- Demais LEDs apagados
- Indica parada obrigatória

### 1.4 Estados Especiais

#### 1.4.1 Botão de Travessia
Quando o semáforo está vermelho e o botão é pressionado:
- Aguarda 1 segundo
- Muda para verde para permitir travessia
- Retorna ao ciclo normal

#### 1.4.2 Alerta de Emergência
Quando o botão é pressionado 3 vezes durante o sinal vermelho:
- Sistema envia requisição HTTP
- Monitor serial mostra código de resposta
- Continua operação normal

## 2. Condições de Transição

### 2.1 Transições de Modo
- De Normal para Noturno: Quando LDR ≤ 600
- De Noturno para Normal: Quando LDR > 600

### 2.2 Transições de Estado (Modo Normal)
- Verde → Amarelo: Após 3 segundos
- Amarelo → Vermelho: Após 2 segundos
- Vermelho → Verde: Após 5 segundos ou 1 segundo após pressionamento do botão

## 3. Tratamento de Eventos

### 3.1 Debounce do Botão
- Implementado com delay de 50ms
- Evita leituras falsas por ruído elétrico
- Garante precisão na contagem de pressionamentos

### 3.2 Contador de Pressionamentos
- Conta pressionamentos apenas durante sinal vermelho
- Reinicia após envio do alerta
- Requer 3 pressionamentos para ativar alerta

## 4. Observações Técnicas

### 4.1 Conexão WiFi
- Necessária para envio de alertas
- Sistema aguarda conexão antes de iniciar operação
- Mostra status no monitor serial

### 4.2 Sensor de Luminosidade
- Leitura digital do LDR
- Threshold configurável (atualmente 600)
- Determina modo de operação do sistema