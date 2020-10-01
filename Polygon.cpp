#include "Polygon.h"

int compForPoint(Point*& a, Point*& b) {
	return (a->x != b->x) ? (a->x > b->x) : (a->y < b->y);
}

Polygon::Polygon() {

}
Polygon::~Polygon() {

}

void Polygon::setPlayer(int player) {
	_player = player;
}
void Polygon::setId(int id) {
	_id = id;
}
int Polygon::getPlayer() {
	return _player;
}
int Polygon::getId() {
	return _id;
}

void Polygon::wrap(set<Point*>& circuit, vector< vector<int> >& graf, vector<Point>& points) {
	for (auto i : circuit)
		i->is_inside = false, i->in_hull = false;
	_hull.clear();
	Point* startPoint = *circuit.begin();
	for (auto i : circuit) {
		startPoint = (startPoint->x != i->x) ? (startPoint->x < i->x) ? i : startPoint
											 : (startPoint->y > i->y) ? i : startPoint;
	}
	Point* point;
	Point* now = startPoint;
	Point* prev = startPoint;

	double minAngleAfter180, minAngle, angle;
	Point* minAnglePoint = startPoint;
	Point* minPointAfter180 = startPoint;
	set<int> alreadyLook;
	bool not_first = false;
	do {
		now->polygon = this->_id;
		now->in_hull = true;
		alreadyLook.insert(now->id);
		_hull.push_back(now);
		minAngle = 361.0;
		minAngleAfter180 = 361.0;
		minPointAfter180 = NULL;
		for (int index : graf[now->id]) {
			point = &points[index];
			if (alreadyLook.find(point->id) != alreadyLook.end()) continue;
			if (circuit.find(point) == circuit.end()) continue;
			angle = getAngle(now, point);
			if (angle < minAngle) {
				minAngle = angle;
				minAnglePoint = point;
			}
			if (angle >= 180.0 && angle <= minAngleAfter180) {
				minAngleAfter180 = angle;
				minPointAfter180 = point;
			}
		}
		prev = now;
		if (minAngle < 45.0 && not_first && minPointAfter180 != NULL) now = minPointAfter180;
		else now = minAnglePoint;
		not_first = true;
	} while (now != prev);
}

bool Polygon::is_inside(Point* point) {
	std::pair<Point, Point> firstEdge = std::make_pair(*point, Point(INT_MAX, point->y, -1));
	std::pair<Point, Point> secondEdge;
	int count_intersects(0);
	bool ret = false;

	for (int i = 0; i < _hull.size(); i++) {
		if (i == _hull.size() - 1) secondEdge = std::make_pair(*(_hull[i]), *(_hull[0]));
		else secondEdge = std::make_pair(*(_hull[i]), *(_hull[i+1]));
		if (point->y == secondEdge.first.y && point->y == secondEdge.second.y) continue;
		if (point->y == secondEdge.first.y || point->y == secondEdge.second.y) {
			firstEdge.first.y++;
			ret = true;
		}
		
		if (edges_intersect(firstEdge, secondEdge)) count_intersects++;
		if (ret) {
			firstEdge.first.y--;
			ret = false;
		}
	}
	return (count_intersects % 2) ? true : false;
}
void Polygon::addPoint(Point* point) {
	point->player = this->_player;
	point->polygon = this->_id;
	point->is_inside = true;
	point->connect = this->_hull[0]->connect;
	//this->_vertex.push_back(point);
}

bool Polygon::operator==(Polygon& right) {
	return this->_id == right._id;
}

void Polygon::pool(Point* one) {
	_hull.push_back(one);
}