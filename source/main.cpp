#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Point {
    double x, y;
    int conjunto;
    double menor_distancia;
    Point() : x(0.0), y(0.0), conjunto(-1), menor_distancia(DBL_MAX) {}
    Point(double x, double y) : x(x), y(y), conjunto(-1), menor_distancia(DBL_MAX) {}
    
    double distancia(Point p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};

vector<Point> leerCSV() {
    string trash;
    cout << "leyendo\n\n";
    vector<Point> pts;
    ifstream file("DATASET.csv");
    if (!file.is_open()) {
        cout << "ERROR: archivo abierto\n";
    }
    //ignorar las primeras 2 lineas (cabecera)
    for (int i = 0; i < 2; i++) {        
        getline(file, trash, '\n');
    }   
    //data real j < n puntos, 14.5M
    int j = 0;
    while (file.good() && j<14500000) {
        double x, y;
        string Start_Lon, Start_Lat;
        //ignorar los primeros 3 datos
        for (int k = 0; k < 5; k++) {
            getline(file, trash, ',');
        }
        //datos que nos importan
        getline(file, Start_Lon, ',');
        getline(file, Start_Lat, ',');
        
        //ignorar el resto
        getline(file, trash, '\n');

        x = stod(Start_Lon);
        y = stod(Start_Lat);
        
        pts.push_back(Point(x, y));
        j++;        
    }
    file.close();
    return pts;
}

void kmeans(vector<Point>* pts, int repeticiones, int k) {
    int n = pts->size();

    vector<Point> p_random;
    srand(time(NULL));
    for (int i = 0; i < k; ++i) {
        p_random.push_back(pts->at(rand() % n));
    }
    for (int i = 0; i < repeticiones; ++i) {

        for (vector<Point>::iterator c = begin(p_random); c != end(p_random); ++c) {
            int clusterId = c - begin(p_random);

            for (vector<Point>::iterator iterador = pts->begin();
                iterador != pts->end(); ++iterador) {
                Point p = *iterador;
                double D = c->distancia(p);
                if (D < p.menor_distancia) {
                    p.menor_distancia = D;
                    p.conjunto = clusterId;
                }
                *iterador = p;
            }
        }

        vector<int> nPoints;
        vector<double> total_x, total_y;
        for (int j = 0; j < k; ++j) {
            nPoints.push_back(0);
            total_x.push_back(0.0);
            total_y.push_back(0.0);
        }
        
        for (vector<Point>::iterator iterador = pts->begin(); iterador != pts->end();++iterador) {
            int clusterId = iterador->conjunto;
            nPoints[clusterId] += 1;
            total_x[clusterId] += iterador->x;
            total_y[clusterId] += iterador->y;

            iterador->menor_distancia = DBL_MAX;  
        }
        
        for (vector<Point>::iterator c = begin(p_random); c != end(p_random); ++c) {
            int clusterId = c - begin(p_random);
            c->x = total_x[clusterId] / nPoints[clusterId];
            c->y = total_y[clusterId] / nPoints[clusterId];
        }
    }

    // archivo de salida
    cout << "escribiendo\n";
    ofstream myfile;
    myfile.open("output.csv");
    myfile << "x,y,c" << endl;

    for (vector<Point>::iterator iterador = pts->begin(); iterador != pts->end();++iterador) {
        myfile << iterador->x << "," << iterador->y << "," << iterador->conjunto << endl;
    }
    myfile.close();
}

int main() {
    vector<Point> pts = leerCSV();
    kmeans(&pts, 100, 6);
    return 0;
}