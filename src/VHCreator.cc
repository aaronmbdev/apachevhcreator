
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/wait.h>
#include <fstream>
#include <algorithm>

using namespace std;

void usage() {
    cout << "Apache VH Creator" << endl;
    cout << "Uso: ./VHCreator midominio.com nombrefolder" << endl;
    cout << "Esto creará la configuración para midominio.com en /var/www/nombrefolder" << endl;
    cout << "Si no se especifica nombrefolder, se usa el nombre del dominio" << endl;
    exit(0);
}

string getPath(const string& path) {
    return "/var/www/"+path+"/public_html/";
}

bool requestConfirmation(const string& folder, const string& dominio) {
    cout << "Se creará el virtualhost para el dominio " << dominio << " en el directorio " << folder << " ¿Es correcto? (Y/N)" << endl;
    string resp;
    cin >> resp;
    if(resp == "Y" || resp == "y") {
        return true;
    }
    return false;
}

bool routeExists(const string& s) {
    struct stat buffer;
    return (stat (s.c_str(), &buffer) == 0);
}

void borrarDatos(string& r) {
    int pid = fork();
    if(pid != -1) {
        if(pid == 0) {
            char cstr[r.size() + 1];
            strcpy(cstr,r.c_str());
            execlp("rm","rm","-r",cstr, NULL);
            cout << "Fallo ejecutando comando de limpieza..." << endl;
            exit(1);
        }
    } else {
        cout << "Fallo creando thread para limpiar directorio" << endl;
    }
}

void configuraPermisos(const string& r) {
    int pid = fork();
    if(pid != -1) {
        if(pid == 0) {
            char cstr[r.size() + 1];
            strcpy(cstr,r.c_str());
            execlp("chown","chown","www-data:www-data",cstr, NULL);
            cout << "Fallo ejecutando comando de asignación de permisos..." << endl;
            exit(1);
        }
    } else {
        cout << "Fallo creando thread para asignar permisos" << endl;
    }
}

string getServerAlias(const string& c) {
    size_t n = count(c.begin(),c.end(),'.');
    if(n > 1) {
        //Es subdominio
        return c;
    } else {
        return "www."+c;
    }
}

void moverFicheroConf(const string& r) {
    int pid = fork();
    if(pid != -1) {
        if(pid == 0) {
            char cstr[r.size() + 1];
            strcpy(cstr,r.c_str());
            execlp("mv","mv",cstr,"/etc/apache2/sites-available/", NULL);
            cout << "Fallo ejecutando comando para mover el fichero a apache..." << endl;
            exit(1);
        }
    } else {
        cout << "Fallo creando thread para mover fichero" << endl;
    }
}

void activarApache2(const string& r) {
    int pid = fork();
    if(pid != -1) {
        if(pid == 0) {
            char cstr[r.size() + 1];
            strcpy(cstr,r.c_str());
            execlp("a2ensite","a2ensite",cstr, NULL);
            cout << "Fallo ejecutando comando para activar sitio en apache2..." << endl;
            exit(1);
        }
    } else {
        cout << "Fallo creando thread activar apache" << endl;
    }
}

int main(int argc, char *argv[]) {
    string dominio = "null";
    string folder = "null";
    if(argc == 1 || argc > 3) {
        usage();
    } else {
        dominio = argv[1];
        folder = argv[1];
        if(argc == 3) {
            folder = argv[2];
        }
        folder = getPath(folder);
        if(requestConfirmation(folder,dominio)) {
            cout << "Creando directorio..." << endl;
            char cstr[folder.size() + 1];
            strcpy(cstr,folder.c_str());

            string preFolder = folder;
            preFolder.replace(preFolder.find("/public_html/"),12,"");
            char preFolderchar[preFolder.size() + 1];
            strcpy(preFolderchar,preFolder.c_str());
            if(routeExists(folder)) {
                cout << "Parece que la ruta " << folder << " ya existe, ¿Quieres eliminar su contenido? (Y/N)" << endl;
                string borrar;
                cin >> borrar;
                if(borrar == "Y" || borrar == "y") borrarDatos(folder);
            }
            while(waitpid(-1,NULL,WNOHANG) == 0);
            int checkPre = mkdir(preFolderchar,0775);
            int check = mkdir(cstr,0775);
            if(checkPre == -1 || check == -1) {
                cout << "No se ha podido crear el directorio " << folder << ". Verifique los permisos del programa y vuelva a ejecutar." << endl;
                exit(0);
            }
            cout << "Carpetas configuradas correctamente (1/4)" << endl;
            cout << "Configurando permisos..." << endl;
            configuraPermisos(folder);
            while(waitpid(-1,NULL,WNOHANG) == 0);
            cout << "Permisos configurados (2/4)" << endl;
            cout << "Creando fichero " << dominio << ".conf " << endl;

            string file = dominio+".conf";
            char filename[file.size() + 1];
            strcpy(filename,file.c_str());
            ifstream infile("src/template.conf");
            ofstream fileout(filename);
            string aux;
            while(getline(infile,aux)) {
                if(aux.find("%SERVERNAME%") != string::npos) {
                    aux.replace(aux.find("%SERVERNAME%"),12,dominio);
                } else if(aux.find("%SERVERALIAS%") != string::npos) {
                    string alias = getServerAlias(dominio);
                    aux.replace(aux.find("%SERVERALIAS%"),13,alias);
                } else if(aux.find("%DOCROOT%") != string::npos) {
                    aux.replace(aux.find("%DOCROOT%"),9,folder);
                }
                aux += "\n";
                fileout << aux;
            }

            cout << "Fichero de configuración creado (3/4)" << endl;

            cout << "Registrando sitio en apache2 ..." << endl;
            moverFicheroConf(filename);
            while(waitpid(-1,NULL,WNOHANG) == 0);
            activarApache2(filename);
            while(waitpid(-1,NULL,WNOHANG) == 0);
            cout << "Script Finalizado (4/4)" << endl;


        } else {
            cout << "Ejecución abortada..." << endl;
        }
    }
}