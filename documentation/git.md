# Git
Git ist eine Software zur verteilten Versionsverwaltung von Dateien. Wenn man noch keine Ahnung von Git hat, kann [ich](@Leander.Schulten) das [Git Book](https://git-scm.com/book/en/v2) empfehlen, zumindest die Kapitel [Recording Changes to the Repository](https://git-scm.com/book/en/v2/Git-Basics-Recording-Changes-to-the-Repository), [Branches in a Nutshell](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) und [Basic Branching and Merging](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging). Man kann natürlich auch jedes andere Turoial im Internet lesen.

### Git installieren
*Linux:* `sudo apt install git`  
*Mac:* Wenn XCode installiert ist: `sudo xcode-select --install`, wenn [brew](https://brew.sh/) installiert ist: `brew install git`, sonst von der [Git Webseite](https://git-scm.com/downloads)  
*Windows:* Lade das _Git for Windows SDK_ [hier](https://github.com/git-for-windows/build-extra/releases) herrunter. Das normale _Git for Windows_ von der [Git Webseite](https://git-scm.com/downloads) reicht nicht aus, da dort nicht alle Tools enthalten sind, die für das Bauen der Abhängigkeiten benötigt werden!  

### SHH-Key auf git.rwth-aachen.de registrieren
Um ein git repository klonen, sowie pullen und pushen zu können, muss ein lokaler SSH-Key auf git.rwth-aachen.de registriert werden.  
Gehe dafür auf https://git.rwth-aachen.de/profile/keys und befolge die dort oben verlinkte Anleitung, die sehr genau ist.  
Unter **Windows** kann es zu Problemen kommen, wenn man mit den Key in SmartGit importieren möchte. Erfahrungsgemäß funktionieren Keys, die mit [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) erzeugt wurde (bis zum Download von *puttygen.exe* runterscrollen). In puttygen.exe dann einfach einen Key generieren und privaten und öffentlichen Schlüssel speichern. 

### SmartGit installieren
Git bietet von sich aus nur ein Kommandozeilenprogram an. Wenn man eine grafische Oberfläche haben möchte, kann man z.B. [SmartGit](https://www.syntevo.com/smartgit/) installieren.
1. Download SmartGit from [here](https://www.syntevo.com/smartgit/download/). Linux: Use the *Debian Bundle*
2. Start the installer
3. Choose *Non-commercial use only (most features, no support)* and press *Next*
4. Wait 10 seconds, check the boxes and click *OK*
5. Enter your real name and a real email. The name is used in the commits and the email is used to send you a mail when you commit broken code. *Next*
6. Select *Use SmartGit as SSH client*. *Next*
7. Select *File Manager*. *Next*
8. Press *Finish*
9. Select *Clone existing repository*. *OK*
10. Repository URL: `git@git.rwth-aachen.de:leander.schulten/Lichtsteuerung.git`. *Next*
11. If a dialog opens: Select the file that you have created in the step *SHH-Key auf git.rwth-aachen.de registrieren*
12. *Next*. *Finish*