# LP25
Lien vers le sujet:
https://github.com/KodjoTossou/lp25-a25/

Commandes pour installer ncurses :

sudo apt install libncurses5-dev 

une fois le programe compilé il s'execute avec ./mytop

si il y a des problèmes de droit d'execution pour ./mytop , faites : "make clean" dans la console de comande puis refaire "make".


pour instaler le réseau SSH
sur la machine distante:
-verifier que le serveur est fonctionel avec : sudo systemctl status ssh

machine local:
-vérifiez que la commande : "sudo apt install sshpass" fonctionne 
-si vous utilisez le fichier ".mes_serveurs" vérifiez bien que les droits sont "chmod 600 .mes_serveurs".
