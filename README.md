
## ENUNCIADO PRÁCTICA
#### DESCRIPCIÓN

El objetivo de la práctica es que el alumno se familiarice con la sincronización de procesos y threads. Para
ello, se va a plantear un problema en el que se simulará una aplicación que simule la gestión y
funcionamiento de BiciMAD.

BiciMAD es un medio de transporte público urbano de la ciudad de Madrid prestado con bicicletas
eléctricas. En concreto, los usuarios del servicio pueden coger las bicis que se encuentran en N estaciones
establecidas por todo Madrid. El programa a implementar debe gestionar mediante threads el
funcionamiento de múltiples usuarios que acceden a las diferentes N estaciones aleatoriamente siguiendo
su propio criterio (se puede utilizar la función rand() para seleccionar la estación a la que acuden) para
conseguir una bici para realizar su trayecto a otra estación. Cada estación tendrá determinado un número
de huecos para bicis comenzando la ejecución con 3/4 de los huecos llenos (bicis aparcadas). Los usuarios,
una vez cogida una bici estarán un tiempo aleatorio montando y elegirán otra estación de forma aleatoria
donde soltar la bici. Si un usuario llega a una estación y no hay bicis se quedará esperando hasta que otro
usuario traiga una bici hasta esa estación. De igual forma si un usuario llega a una estación y no encuentra
huecos disponibles para dejarla esperará hasta que otro usuario se llevé una bici y generé un hueco.
Después de dejar la bici, los mismos usuarios pueden volver pasado un cierto tiempo aleatorio a por otra
bici.

Por limitaciones de tiempo y espacio, para la implementación se utilizará un número reducido de
habitantes y estaciones.

Se utilizará un fichero de texto como entrada para saber el número total de usuarios, el número de
estaciones, el número de huecos en cada estación, así como el tiempo de espera hasta que un usuario
decide coger una bici y el tiempo que pasa montando en ella. Este fichero tendrá únicamente un número
entero por línea en el siguiente orden:
- Usuarios totales (400)
- Número de estaciones (12)
- Número de huecos por estación (20)
- Tiempo mínimo de espera para decidir coger una bici (1). Nunca podrá ser menor de 1.
- Tiempo máximo de espera para decidir coger una bici (4)
- Tiempo mínimo que pasa un usuario montando una bici (1). Nunca podrá ser menor de 1.
- Tiempo máximo que pasa un usuario montando una bici (5)
El nombre del fichero de entrada se pasará como argumento del programa.

La salida se presentará por pantalla y también se almacenará en un fichero cuyo nombre se pasará por
argumento al programa.

#### OBJETIVOS PARCIALES

    1. Simular correctamente el sistema BiciMAD mediante threads. Para dar por correcto el funcionamiento de este apartado, el programa tiene que estar libre de interbloqueos y no producir inanición.

    2. Leer la configuración inicial del problema (usuarios, número estaciones, etc) desde un fichero de texto.

    3. Escribir por pantalla cómo se desarrolla el proceso de gestión de las bicis indicando la configuración inicial del problema, la estación donde coje la bici cada usuario y dónde la deja.

    4. Escribir la información anterior en un fichero de texto.

    5. Pasar por argumentos los nombres de los ficheros de entrada y de salida. En caso de que no se introduzca el fichero de salida, su nombre por defecto será salida_sim_BiciMAD<hora y fecha>.txt, donde <hora y fecha> será la hora y fecha de cuando empezó la simulación. Si no se pasa el nombre del fichero de entrada, su nombre por defecto será entrada_BiciMAD.txt. Es decir, el programa tiene que entender:
        biciMAD
        biciMAD nombre_fichero_entrada
        biciMAD nombre_fichero_entrada nombre_fichero_salida
    En el caso en que se introduzca un nombre para el fichero de salida, este nombre deberá incluir la hora y la fecha al principio del nombre. Por ejemplo, si se introduce salida_sim.txt, el programa debe crear el fichero <hora y fecha>salida_sim.txt.

#### NORMAS DE ESTILO

    1. El código fuente se evaluará por su simplicidad, la presencia de comentarios, la óptima gestión de recursos y la gestión de errores. A la hora de codificar la solución pedida, se deberán respetar una serie de normas de estilo:

    2. Las variables locales de las funciones se declararán inmediatamente después de la llave de comienzo del cuerpo de la misma. Se penalizará la declaración de variables entre sentencias ejecutables de las funciones.

    3. No se admitirán asignaciones iniciales a variables cuyo valor dependa del valor de otras variables. El valor asignado en estos casos siempre deberá ser conocido en tiempo de compilación.

    4. Cuando se declare una variable de tipo array, su tamaño deberá ser conocido en tiempo de compilación. Si se quiere utilizar un array de tamaño variable, deberá crearse en memoria dinámica mediante las funciones correspondientes (malloc, calloc o realloc).

    5. Las operaciones sobre strings (copia, comparación, duplicación, concatenación, etc) se realizarán en lo posible mediante las funciones indicadas en string.h 

    6. En general, se penalizará el uso de construcciones propias de C++.

    7. Al compilar el código fuente, deberá producirse el menor número posible de warnings (mejor que no se produzca ninguno).

