### Install python-flask environment
https://linuxize.com/post/how-to-install-flask-on-ubuntu-20-04/


### Env acivate alias
```console
alias activate="source /home/$USER/proj/cs50/bin/activate"
```


### Run STM32CubeMX from any place
```console
alias cubemx="/usr/local/STMicroelectronics/STM32Cube/STM32CubeMX/STM32CubeMX > /dev/null 2>&1 &"
```


### Setup ARM Compiler and ST Flash Tools
https://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-stm32-chip-on-linux-ubuntu/
st-link is better to install from ubuntu repos:
```consol
sudo apt-get install stlink-tools
sudo ldconfig
```

After that you are able to use:
 * st-info to probe your debugger
    - st-info --version
    - st-info --flash
    - st-info --sram
    - st-info --descr
    - st-info --pagesize
    - st-info --chipid
    - st-info --serial
    - st-info --hla-serial
    - st-info --probe
 * st-flash to burn a firmware

### Pinout
**LEDS:**
   - PD12 
   - PD13
   - PD14
   - PD15

**Input buttons:**
   - PA0

**Relays:**
   - PA1

**UART for WEB communication:**
   - PA2 - USART2_TX
   - PA3 - USART2_RX

**OLED Display:**
   - PE0 - PE7 - parallel lines for data bits
   - PE8 - R/W command line
   - PE9 - Enable strob
   - PE10 - RS (defines either data or instruction on parallel lines)
