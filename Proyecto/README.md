# Funcionamiento del sistema
Mineros:  La red se compone de un conjunto de mineros:

 - Cada minero está formado por dos procesos: el proceso padre, Minero, encargado de resolver la POW usando múltiples hilos en paralelo, y el proceso hijo, Registrador, encargado de mantener actualizado un fichero de registro. Estos procesos se comunican por medio de una tubería.
 - El minero que consiga ser el primero en resolver la POW solicita al resto de mineros que validen su solución mediante una votaci´on, obteniendo una moneda si se aprueba. Además, envía el bloque resultante al monitor, si está activo, a trav´es de una cola de mensajes.
 - Los mineros se pueden lanzar desde una misma shell, o desde shells diferentes.
 - Una vez se ha lanzado el primer minero, cualquier otro minero lanzado se une a la red de mineros.
 - Los mineros pueden unirse en cualquier momento.
 - Los mineros pueden acabar y salir de la red en cualquier momento.
 - La información sobre la red debe adaptarse de forma dinámica a los cambios anteriores.
 - El funcionamiento de la red debe ser robusto frente a altas o bajas de mineros, en particular se prestará atención a la validez de la información compartida y a evitar bloqueos (deadlocks).

Nota: Para simplificar la codificación, se permite suponer que el número máximo de mineros que van a participar en el proceso de minado de una red nunca excederá una determinada constante (un valor razonable inicial para esta constante es de 100). A partir de ese valor, el sistema se considera lleno y se rechazan los nuevos mineros.

Monitor:  Opcionalmente, el sistema puede contar con un monitor, que permite a los mineros enviar datos informativos  sobre la ejecución del sistema, de manera que se cree una salida única para toda la red.

 - El monitor está formado por dos procesos, el proceso padre, Comprobador, encargado de recibir los bloques por cola de mensajes y validarlos, y el proceso hijo, Monitor, encargado de mostrar la salida unificada. Estos procesos se comunican usando memoria compartida y un esquema de productor–consumidor.
 - El monitor podr´a arrancar en cualquier momento.
 - El monitor podr´a terminar en cualquier momento, e incluso volver a arrancar m´as adelante.
 - La red debe ser capaz de adaptarse a estos cambios, de forma que no se acumulen mensajes pendientes si no hay un monitor activo.

Para implementar este sistema, se escribir´an dos programas en C, uno correspondiente al
minero y otro al monitor.

# Recursos de la red
Bloque: Cada desafío resuelto se almacena en un bloque, que contiene, entre otros, los siguientes
datos:
 - Un identificador del bloque, que se incrementa en cada ronda.
 - El target, resultado del que se desea hallar el operando.
 - La soluci´on, operando que permite hallar el target.
 - El PID del minero ganador, el que ha resuelto la POW.
 - Las carteras de los mineros actuales, incluyendo sus PID y las monedas que han obtenido hasta ese momento.
 - El n´umero de votos totales que se han usado para evaluar ese bloque, y el n´umero de votos positivos.

Segmento de memoria del sistema: En memoria compartida se almacena una estructura con la informaci´on del sistema, entre otros:
 - Un listado con los PID de los mineros activos.
 - Un listado con los votos de cada minero.
 - Un listado con las monedas obtenidas por cada minero.
 - El ´ultimo bloque resuelto, y el bloque actual que se est´a resolviendo.
Adem´as, se pueden incluir los sem´aforos que se consideren necesarios para sincronizar el sistema.

Cola de mensajes: Se utiliza una cola de mensajes para realizar la comunicaci´on entre los
mineros y el monitor.
 - Tuber´ıas La comunicaci´on entre los dos procesos que conforman el minero (Minero y Registrador) se hace usando tuber´ıas.
 - Segmento de memoria de monitoreo La comunicaci´on entre los dos procesos que conforman el
monitor (Comprobador y Monitor) se hace usando una cola circular almacenada en un segmento de
memoria compartida diferente al segmento del sistema. Adem´as, se usa un esquema productor–
consumidor.
