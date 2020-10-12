# Preguntas y respuestas frecuentes (F.A.Q)

### ¿Por qué debo poner obligatoriamente mí usuario y contraseña del sitio de Picta?

Es obligatorio porque para obtener las listas de reproducción del sitio y descargar sólo puede ser con un usuario registrado.

### ¿Se guardan de forma segura los datos?

Si, la contraseña se guarda encriptada.

### ¿Puedo conectarme a través de un Proxy con usuario y contraseña?

La aplicación de momento permite conectarse usando un proxy por la dirección IP y puerto pero no esta implementada la autenticación con usuario y contraseña, le recomendamos que use algún programa que proxifique su conexión como Proxifier, ProxyCap u algun otro. Quizás en futuras versiones se implemente este tipo de autenticación.

### ¿Porque aparecen deshabilitadas las opciones de calidad de vídeo Media y baja?

Para poder obtener los vídeos en esas calidades se necesita conocer el formato con que se publican en el sitio  de Picta y de momento picta-dl no puedo obtenerlos, cuando este detalle se arregle se activaran esas opciones

### ¿Puedo usar este programa en sistemas que sean de 32 bits?

Lamentablemente no, el programa picta-dl está compilado solo para versiones de 64bits de Windows

### Tengo Windows 10 version 2004 recien instalado y me da problemas al descargar

Si, parece que las versiones recientes de Windows 10 cuando el sistema está recien instalado sin parches de seguridad da problemas de certificados de conexión, estamos trabajando en resolver ese problema.

### ¿Cómo creo las listas de reproducción para descargarlas?

En el sitio de Picta puede crear listas personalizadas con los videos que Ud. seleccione, en cada video le aparecerá un icono para agregar los contenidos a una lista ya creada o si no tiene ninguna puede crearlas en ese momento y adicionarlos, posteriormente puede acceder a esa lista desde su perfil y pasar la URL de esta a este programa y después de procesarla verá todo el contenido y la podrá descargar. 

![icon list](Resources/Ayuda/icono_de_lista.png)

### ¿Puedo descargar más de una lista de reproducción?

Si, cuando quiera descargar una lista solo pegue la URL, procese, descargue y así por cada lista que tenga creada o de las que ya están creadas en el sitio.

### ¿Cómo identifico las listas de reproducción en el sitio de Picta?

Las listas creadas en el sitio por las personas que suben contenidos a los canales se pueden identificar por el icono que se muestra a la derecha donde se ve la lista de los vídeos que contiene, cuando copia esa URL y la pasa a esta aplicación y la procesa verá el listado de los mismos, aquí una imagen de una lista de reproducción: 

![picta channel list](Resources/Ayuda/picta_lista_canal.png)

### ¿Debo instalar algo adicional para que funcione esta aplicación?

No, ya todo lo necesario para que funcione viene con ella, el __picta-dl__ y __FFmpeg__ para procesar los vídeos.
