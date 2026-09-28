Drop `.8xp` program files here to make them show up in the calculator's APPS
menu. Just put the file in this folder; the server lists the directory at
startup and sends the selected file's bytes to the ESP32, so add or remove files
before starting the server.

The menu item number shown on the calculator is the file's position in the
alphabetical listing, and the name it displays is the first 10 characters of the
filename, upper-cased. With an empty folder the APPS menu simply shows blanks.
