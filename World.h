#pragma once

#include <vector>
#include <set>
#include <map>
#include <list>

#include <cmath>
#include <algorithm>

#include "Polygon.h"

using std::vector;
using std::list;
using std::pair;
using std::set;
using std::map;


class World {
	public:
		const int xMAX = 5000;
		const int yMAX = 5000;

		double capturDistance;
		unsigned TOTAL;
		int playerNow;
		unsigned lastConnectId = 0;
		int free;

		vector< vector <pair<double, int> > > dist; // first - дистанция, second - номер в points (id)
		vector< vector<int> > graph;

		vector<Point> points;
		set<pair<int, int> > edges;
		vector<Polygon> polygons;

		World();
		~World();

		void createWorld(unsigned total);
		void preparingCircuit(Point* const target, Point* const nextPoint, set<Point*>&);
		void createPolygon(Point* const target, Point* const nextPoint);
		vector<int> unitePolygon(set<Point*>& circuit);
		void changeWorld(Polygon* polygon, vector<int>& needUnite);
		void createEdges(int newPointID);
		void unitePolygon(int deletePolygonID, int newPolygonID);
		void deletePolygon(int deletePolygonID);
		void deleteEdges(int deletePointID);

		void updateLineConnect(int oldId, int newId);
		void updatePointConnect(Point* point, int newId);
		void getDistance();
		void buildGraph();
		void updateGraph(const Point*, const Point*);

		bool haveFree();

		vector<Point*> getNearestPoints(Point* point);
};