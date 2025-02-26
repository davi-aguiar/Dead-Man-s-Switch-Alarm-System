# Sistema de Alarme Dead Man's Switch

## Descrição

O **Sistema de Alarme Dead Man's Switch** é um dispositivo embarcado desenvolvido utilizando o Raspberry Pi Pico que visa monitorar a presença ativa do usuário por meio de um botão. Caso o usuário não interaja com o sistema dentro de um período pré-definido, o dispositivo aciona um alarme sonoro e visual, alertando sobre a inatividade. As informações são exibidas em um display OLED, enquanto LEDs e um buzzer fornecem feedback imediato.

## Objetivos

- **Monitoramento de Atividade:** Garantir que o usuário esteja presente e ativo através da verificação periódica de um botão.
- **Segurança:** Acionar um alarme para prevenir situações de risco em que a ausência de resposta pode indicar um problema.
- **Feedback Visual e Sonoro:** Utilizar LEDs, um buzzer e um display OLED para fornecer informações claras sobre o estado do sistema.
- **Gestão de Estados:** Implementar uma máquina de estados que gerencia os diferentes modos de operação (espera, alarme ativo e alerta).

## Componentes Utilizados

- **Microcontrolador:** Raspberry Pi Pico (ou compatível)
- **Display OLED:** SSD1306 (com comunicação I2C)
- **Buzzer:** Controlado via PWM
- **LEDs:**
  - LED Verde: Indica que o sistema está ativo e ok.
  - LED Vermelho: Indica alerta.
- **Botão de Entrada:** Utilizado para reiniciar o sistema e confirmar a presença do usuário.
- **Conexão I2C:** Para comunicação com o display OLED

## Funcionamento

1. **Modo de Espera:**

   - O sistema inicia e exibe "Sistema Ativo" e "Aguardando..." no display OLED.
   - Durante 30 segundos, o sistema aguarda a interação do usuário (pressionamento do botão).

2. **Ativação do Alarme:**

   - Se o botão não for pressionado durante o período de espera, o sistema entra no modo de alarme ativo.
   - O buzzer é ativado com uma frequência definida e o LED vermelho permanece aceso durante 10 segundos.
   - Uma mensagem indicando a necessidade de intervenção é exibida no OLED.

3. **Modo de Alerta:**

   - Se, após o período de alarme, o botão ainda não for pressionado, o sistema entra no modo de alerta.
   - Neste modo, o buzzer e o LED vermelho piscam alternadamente por 5 segundos para enfatizar a situação de alerta.
   - Durante este período, o display OLED exibe mensagens de alerta.

4. **Reinicialização:**
   - Ao pressionar o botão a qualquer momento, o sistema é reiniciado para o modo de espera, os alarmes são desativados e uma mensagem de reinicialização é exibida no display.

## Aplicações

- **Segurança Industrial:** Monitoramento da presença de operadores em ambientes críticos.
- **Sistemas de Veículos:** Garantia de que o operador esteja atento e ativo durante a operação.
- **Equipamentos Sensíveis:** Prevenção de acidentes onde a ausência de resposta imediata pode causar danos.
- **Monitoramento Pessoal:** Dispositivos de segurança para alertar em situações de emergência.

## Instalação e Uso

### Requisitos

- **Hardware:**
  - Raspberry Pi Pico (ou microcontrolador compatível)
  - Display OLED (SSD1306, comunicação I2C)
  - Buzzer com suporte a PWM
  - LEDs (verde e vermelho)
  - Botão (configurado com pull-up interno)
- **Software:**
  - SDK do Raspberry Pi Pico
  - Biblioteca SSD1306 para controle do display OLED

### Conexões

- **Botão:** Conectado ao GPIO 5 (entrada com pull-up)
- **Buzzer:** Conectado ao GPIO 21 (saída PWM)
- **LED Verde:** Conectado ao GPIO 11 (saída)
- **LED Vermelho:** Conectado ao GPIO 13 (saída)
- **Display OLED:**
  - SDA → GPIO 14
  - SCL → GPIO 15

### Compilação e Execução

1. **Clone o repositório:**
   ```bash
   git clone <https://github.com/davi-aguiar/Dead-Man-s-Switch-Alarm-System.git>
   ```
