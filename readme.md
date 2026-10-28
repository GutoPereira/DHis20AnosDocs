# DHis 20 Anos - Docs

## Arduino

### Configuração

O Array `buttonPins` contém a ordem da leitura dos pinos digitais do Arduino. Para esta instalação, a primeira porta recomendada é `D04` e a última `D11`. Comentários na mesma linha indicam a qual botão(1 a 8) eles se referem.

```cpp
const uint8_t buttonPins[N_BUTTONS] = {
  4,  // Botao 1
  5,  // Botao 2
  6,  // Botao 3
  7,  // Botao 4
  8,  // Botao 5
  9,  // Botao 6
  10, // Botao 7
  11  // Botao 8
};
```

A ordem dos botões pode ser definida livremente.

> [!NOTE]
> Tome nota do baudrate definido aqui, pois será utilizado posteriormente pelo App, descrito mais adiante.
> 
> ```cpp
> Serial.begin(115200);
> ```

## App

### Configuração

As configurações do app são feitas a partir de um arquivo XML dentro da pasta data, chamado `config.xml`. Ele possui o nó que engloba toda a aplicação, `App`, e seus filhos, um para cada uma dos 8 vídeos, usando a tag com auto fechamento `video`.

```xml
<App
    fadeTime="2000"
    arduinoPort="COM4"
    baudrate="115200"
    fullscreen="true"
>
    <video file="ANTROPOCRUZO - ABOUT THE EXHIBITION.mp4"/>
    <video file="ANTROPOCRUZO DOCUMENTARY.mp4"/>
    <video file="Documentario da Oficina Grande Rio 2025.mp4"/>
    <video file="TEASER CARNAVAL 2023.mp4"/>
    <video file="Evento DESIGN, BATE-BOLAS E BATE-BOLETES Cultura em Movimento.mp4"/>
    <video file="Documentario da Oficina Grande Rio 2022.mp4"/>
</App>
```

Atributos da tag `App`:

| Atributo | Tipo | Função |
| --- | --- | --- |
| `fadeTime` | int | Tempo em milissegundos de transição dos vídeos |
| `arduinoPort` | string | Nome da porta do arduino, como, por exemplo, `COM3` ou `/dev/cu.usbmodemXXXX` |
| `baudrate` | int | Taxa de comunicação serial do Arduino |
| `fullscreen` | bool | Define se o programa inicia em modo tela cheia ou não |

Atributos da tag `video`:

| Atributo | Tipo | Função |
| --- | --- | --- |
| `file` | string | nome do arquivo de vídeo dentro da pasta `/data/videos/` com a extensão `.mp4` |

> [!WARNING]
> O nome dos arquivos não devem conter caracteres especiais, apenas letras maiúsculas e minúsculas, espaços, `-`, `_` e números.

> [!TIP]
> A ordem das tags `video` define a ordem dos vídeos nos botões.

### Mapa de atalhos

| Tecla 1 | Tecla 2 | Função |
| --- | --- | --- |
| `f` | `F` | Ativa e desativa modo tela cheia |
| `d` | `D` | Exibe ou esconde informações de debug |
| `c` | `C` | Alterna entre mostrar o cursor do mouse ou não |
| `Esc` |  | Fechamento do App |

## Comunicação do Arduino para o App

Ao iniciar o App, ele tentará se conectar na porta configurada pelo arquivo `config.xml` usando o atributo `arduinoPort` usando a taxa de comunicação do atributo `baudrate`, ambos da tag `App`.

> [!WARNING]
> Antes de iniciar o App, certifique-se de que a porta do arduino está livre.

Toda vez que o estado de um botão muda, um byte é enviado via `Serial.write` no Arduino, contendo a informação de todos os botões ao mesmo tempo.

O bit mais significativo, o primeiro da esquerda para a direita, é o botão 8, enquanto o menos significativo("último") é o botão 1.

| Byte | Decimal | Significado |
| --- | --- | --- |
| `00000001` | 1 | Botão 1 pressionado |
| `00101001` | 41 | Botões 1, 4 e 6 pressionados |
| `00000000` | 0 | Nenhum botão pressionado |
| `10000000` | 128 | Botão 8 pressionado |

> [!TIP]
> Para fins de teste do App sem um Arduino, as teclas 1 a 8 podem ser utilizadas substituindo os botões 1 a 8.