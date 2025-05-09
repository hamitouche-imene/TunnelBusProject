Les 4 conditions de l’exclusion mutuelle :
1.Une seule direction peut utiliser le tunnel à la fois.

Dans le code :

Le mutex empêche plusieurs threads de modifier les variables critiques (dans_tunnel, sens_tunnel) en même temps.

Le while dans entrer_tunnel() force les bus à attendre si la direction est opposée.

2. Pas d’interblocage :
   Aucun groupe de bus n’attend indéfiniment l’autre groupe.

Dans le code :

Lorsque le tunnel devient vide (dans_tunnel == 0), on réveille tous les bus de l’autre direction avec pthread_cond_broadcast.

Cela évite qu'un groupe de bus ne monopolise le tunnel pour toujours.

3. progression garantie :
   Chaque bus finira par passer.

Dans le code :

Le système de priorité dynamique (on réveille l’autre direction quand le tunnel est vide) permet à tous les bus de progresser.

Aucun bus n’est bloqué indéfiniment.

4. Attente bornée :
   Un bus ne doit pas attendre indéfiniment une ressource.

Dans le code :

L’ordre d’arrivée est respecté car les bus attendent dans une file avec pthread_cond_wait, et sont réveillés équitablement avec broadcast.

Le bus qui attend le plus longtemps passera dès que c’est possible.

  Cet algorithme peut être retenu comme une solution efficace pour protéger le tunnel, car il garantit une gestion correcte de la synchronisation entre les bus circulant
   dans des directions opposées (X -> Y et Y -> X). L'utilisation des mutex permet de gérer les accès concurrents au tunnel de manière sûre, évitant ainsi
    les collisions ou les blocages. Chaque bus attend son tour en respectant les règles de priorité définies, ce qui assure une circulation fluide et ordonnée.
    De plus, l'algorithme prend en compte le fait que plusieurs bus peuvent attendre pour entrer dans le tunnel, et il permet de libérer les ressources de manière
    optimale en signalant lorsque le tunnel devient disponible pour la direction opposée. En somme, il offre une solution fiable et sécurisée pour la gestion du tunnel
    tout en évitant les conflits d'accès, et il est adapté à un environnement avec un nombre modéré de bus.
        
