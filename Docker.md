# Docker/CI
Hier gibt es eine kurze Einführung in Docker und die Gitlab CI. Weiterhin wird erklärt wie das cross compiling funktioniert, bzw. wie man es aufsetzt und aktuell hält.  
[Docker](https://www.docker.com/) ist eine Containervirtualisierungs-Software. Ein Docker Container ist ähnlich zu einer VM, nur dass der Container etwas leichtgewichtiger ist.  

### Docker unter Windows installieren
Um Docker für Windows herunterladen zu können muss man sich auf [Docker.com](https://www.docker.com/) einen Account erstellen. Nachdem man dies getan hat kann man Docker für Windows herunterladen. Nachdem man es installiert hat sollten ein paar Einstellungen gesetzt werden:
 * Unter Shared Drives C auswählen (oder den Laufwerksbuchstaben auf dem man arbeitet)
 * Unter Advanced: 
   * CPUs auf maximal stellen (wir brauchen später recht viel Rechenpower, sonst dauert das bauen Ewigkeiten)
   * Memory: Steht bei mir auf 10 GB
   * Disk image max size: Steht bei mir auf 120 GB

### Neues Image bauen
0. Docker starten wenn es noch nicht im Hintergrund läuft
1. Öffne eine Konsole
2. cd dich in den Lichtsteuerungs Git Ordner
3. Wenn du das Image später hochladen willst, führe `docker login registry.git.rwth-aachen.de` aus
4. Führe zum bauen `docker build -t TAG .` aus, wobei du `TAG` durch `registry.git.rwth-aachen.de/leander.schulten/lichtsteuerung/build_qt_current_qt_version` (current_qt_version durch z.B. 5_12_4 ersetzen) ersetzt wenn du das Image/den Container hochladen willst oder durch z.B. `lichtsteuerung/build_last_qt_version` oder was ähnliches wenn du das Image nur lokal für dich erstellstellen möchtest. Ist seit dem letzten mal eine neue Qt Version herausgekommen ([Blog](https://blog.qt.io/) [Versionsübersicht](https://wiki.qt.io/Template:Release_Information)) ist es notwendig, an den Befehl `--no-cache` anzuhängen, da Docker sonst einfach das Image vom letzten mal wiederverwendet und die alte Qt Version weiter benutzt.  
5. Das Image kann nun mit `docker push TAG` (TAG vom Punkt 4 benutzten) in die Container Registry von GitLab hochgeladen werden. Lese davor [hier](https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/container_registry) die __komplette__ Seite, benutzte vor allem einen vernünftigen Image Namen oder Tag.

## Cross compiling und Dockerfile
Wir wollen auf einem Linux Container die Lichtsteuerung für Windows bauen. Nicht ganz trivial. Gott sei Dank gibt es dafür fertige Tools wie [MXE](https://github.com/mxe/mxe) die auch schon Qt fertig konfiguriert dabei haben. Vorgefertigte Docker-Images für die verschiedenen Betriebssysteme gibt es auch schon von [Dockcross](https://github.com/dockcross/dockcross), aber leider enthielten diese Fehler, weshalb ich ein neues Dockerfile aus dem bestehenden konstruiert habe. 

Wir wollen windows shared x64 Posix haben, also nehmen wir die [Datei](https://github.com/dockcross/dockcross/blob/master/windows-shared-x64-posix/Dockerfile.in) und inkludieren manuell die [Datei](https://github.com/dockcross/dockcross/blob/master/common.windows) für Windows. Wir machen folgende anpassungen:
 * Wenn wir MXE clonen checken wir nicht mehr einen Commit aus. Wir wollen die neuste Version.
 * Wir fügen zur settings.mk noch die Zeile `MXE_PLUGIN_DIRS := plugins/gcc8` hinzu, da wir einen neuen gcc Compiler mit c++17 Unterstützung verwenden  wollen.
 * Am ende setzten wir das WorkDir auf `/usr/src/mxe` und bauen diverse Qt Pakete die wir benötigen. Als letztes setzten wir den Path so, dass die MXE Compiler an erster Stelle stehen. 

## Gitlab CI
Hier wird kurz die  [.gitlab-ci.yml Datei](https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/blob/windows-release/.gitlab-ci.yml) erklärt. Diese Datei wird autoamtisch von GitLab erkannt und für die CI verwendet. 
[Hier](https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/settings/ci_cd#Runners) hinzugefügte Runner führen dann angestoßen von GitLab die in der .gitlab-ci.yml Datei definierten Skripte in dem Image was wir definiert haben aus.   
##### .gitlab-ci.yml Datei:  
In der ersten Zeile referenzieren wir das Image welches wir aus dem Dockerfile erstelllt und in die GitLab Registry hochgeladen haben. In diesem Image werden die Stages abgearbeitet.  
Unter `stages:` definieren welche Stages in welcher Reihenfolge abgearbeitet werden sollen. 
Jede Stage besitzt ein `sript` Feld. Alle dort aufgelisteten Befehle werden der Reihe nach abgearbeitet. Wenn ein Befehl fehlschlägt, fehlt die Stage fehl und alle darauffolgenden Stages werden nicht ausgeführt.
Die Pfade die unter `artifacts` aufgelistet werden können nach erfolgreichem Durchlaufen der Stage heruntergeladen werden. 
Die Stage `version` und `deploy` werden nur ausgefürt, wenn der Commit auf dem `windows-release` Branch liegen. Dies wird durch die Angabe von `only: refs: -windows-release` erreicht. 