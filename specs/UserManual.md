
# Manual de Usuario — DonCEy Kong Jr

## Instalación y Ejecución

Los ejecutables se generan en la carpeta `dist/` tras compilar el proyecto o ejecutar los scripts de build. Cada plataforma tiene su propio subdirectorio:

- **Cliente**: `dist/[plataforma]/DonCEy-Kong-Jr-Client` (o `.app`/`.exe` según sistema)
- **Servidor**: `dist/[plataforma]/DonCEy-Kong-Jr-Server` (o `.app`/`.jar`/`.exe`)

Ambos deben estar abiertos y pueden estar en la carpeta de instalación o lanzados desde accesos directos. El servidor debe iniciarse primero. Luego, abre el cliente en la misma máquina o en otra dentro de la red local.

## Objetivo del Juego

Controla a Donkey Kong Jr. y rescata a Donkey Kong, que está atrapado en una jaula custodiada por Mario. Debes llegar a la meta donde está la jaula para subir de nivel, respawnear, y conseguir una vida adicional (subir de nivel resetea los enemigos, frutas y además hace a los enemigos más rápidos)

## Cómo Jugar

### Navegación en Menús
- Usa las **flechas** (izquierda/derecha/arriba/abajo) para moverte entre opciones.
- Pulsa **Enter** para seleccionar una opción o confirmar.

### Controles en el Juego
- **Izquierda/Derecha**: Mover al personaje lateralmente.
- **Arriba/Abajo**: Subir o bajar por las lianas.
- **Espacio**: Saltar o agarrar una liana cercana.
- **Enter**: Confirmar selección en menús.

### Reglas del Juego
- El objetivo es llegar a la jaula donde está Donkey Kong para pasar de nivel y ganar una vida extra.
- Colisionar con Mario te hace perder una vida.
- Hay diferentes tipos de cocodrilos:
  - **Cocodrilo Rojo**: Se mueve hacia las lianas y oscila arriba y abajo infinitamente.
  - **Cocodrilo Azul**: Se mueve hacia las lianas y cae hasta el agua
- Evita los cocodrilos y otros enemigos.
- Recoge frutas para sumar puntos extra.
- Si pierdes todas las vidas, el juego termina.

## Consejos y Notas Adicionales
- Puedes abrir varias instancias del cliente en diferentes dispositivos conectados por LAN, pero cada instancia es una partida independiente (no hay varios jugadores en la misma partida).
- El juego soporta hasta **2 jugadores** conectados al servidor, y cada jugador puede tener hasta **2 espectadores** que observan su partida en tiempo real.
- El servidor solo debe abrirse una vez por partida.
- El servidor incluye un panel de administración (Server UI) donde el administrador puede generar enemigos y frutas en tiempo real para los jugadores.
- Si tienes problemas de conexión, asegúrate de que ambos ejecutables estén en la misma red y carpeta de instalación.

---
¡Diviértete jugando DonCEy Kong Jr!