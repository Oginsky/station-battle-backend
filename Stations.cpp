#include <fstream>

#include "stdlib.h"

#include <string>

#include "World.h"
#include "Polygon.h"

using namespace std;

void printPolygons(World* world);
int compForNeighbors(Point*& one, Point*& two);

void addPoint(World* world, Point* target, int id) {
	target->player = world->playerNow;
	vector<Point*> neighbors = world->getNearestPoints(target);
	for (auto& i : neighbors)
		world->createEdges(id);

if (neighbors.empty()) {
		world->points[id].connect = (world->lastConnectId)++;
		return;
	}
	if (neighbors.size() == 1) {
		target->connect = neighbors[0]->connect;
	//	world->updateGraph(target, neighbors[0]);
		return;
	}

	for (int i = 0; i < neighbors.size(); i++) {
	//	world->updateGraph(target, neighbors[i]);
		if (target->connect == neighbors[i]->connect) {
			if (target->polygon == neighbors[i]->polygon && neighbors[i]->polygon != -1) continue;
			world->createPolygon(target, neighbors[i]);
		}
		/*соединения не равны*/
		else if (target->connect == -1) 
			target->connect = neighbors[i]->connect;
		else
			world->updateLineConnect(target->connect, neighbors[i]->connect);
	}
	
}

void sendMap(World* world) {
	ofstream fout("0.txt");
	for (int i = 0; i < world->TOTAL; i++)
		fout << i << " " << world->points[i].x << " " << world->points[i].y << endl;
	// id x y
}
void send(World* world) {
	static int i = 0;
	i++;
	string str = to_string(i);
	str += ".txt";
	ofstream fout(str);
	for (int i = 0; i < world->TOTAL; i++) {
	//	if (world->points[i].player != world->playerNow) continue;
		fout << "d" << " " << 0 << " " << world->points[i].player << endl;
		fout << world->points[i].id << endl;
	}
	set<pair<int, int> >::iterator it = world->edges.begin();
	while(it != world->edges.end()) {
		fout << "e" << " " << 0 << " " << world->points[it->first].player << endl;
		fout << it->first << endl;
		fout << it->second << endl;
		it++;
	}
	for (int i = 0; i < world->polygons.size(); i++) {
		if (world->polygons[i]._delete) continue;
		fout << "s" << " " << 0 << " " << world->polygons[i]._hull.size() << " " << world->polygons[i]._player << endl;
		for (int j = 0; j < world->polygons[i]._hull.size(); j++) {
			fout << world->polygons[i]._hull[j]->id << endl;
		}
	}
}

int main() {
setlocale(LC_ALL, "rus");
	const unsigned moves = 150;
	World world;
	world.createWorld(100);
	sendMap(&world);
	/*Ход*/
	int id;
	for (int i = 0; i < moves && world.haveFree(); i++) {
		world.playerNow = 1 + i % 2;
		cin >> id;
		while (world.points[id].player != 0) {
			cout << "already captured\n";
			cin >> id;
		}
		addPoint(&world, &world.points[id], id);
	//	printPolygons(&world);
		send(&world);
	}
	return 0;
}

void printPolygons(World* world) {
	cout << "------------------------------\n";
	for (auto i : world->polygons) {
		if (i._delete) continue;
		cout << i._id << ": ";
		for (int j = 0; j < i._hull.size(); j++) {
			cout << i._hull[j]->id << " ";
		}
		cout << endl;
		for (int j = 0; j < world->TOTAL; j++) {
			if (world->points[j].polygon == i._id && world->points[j].is_inside)
				cout << world->points[j].id << " ";
		}
		cout << endl;
	}
	cout << "------------------------------\n";
}
int compForNeighbors(Point*& one, Point*& two) {
	return one->connect < two->connect;
}
