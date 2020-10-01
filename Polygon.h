#pragma once

#include <iostream>

#include <vector>
#include <algorithm>
#include <set>

#include "Point.h"

using std::vector;
using std::cout;
using std::endl;
using std::set;

class Polygon {
	public:
		vector<Point*> _vertex;
		vector<Point*> _hull;
		int _id;
		int _player;
		bool _delete = false;

	public:
		Polygon();
		~Polygon();

		void setPlayer(int player);
		void setId(int id);
		int getPlayer();
		int getId();
		
		void wrap(set<Point*>& circuit, vector< vector<int> >& graf, vector<Point>& points);
		bool is_inside(Point* point);
		void addPoint(Point* point);

		bool operator==(Polygon& right);

		void pool(Point* one);
};