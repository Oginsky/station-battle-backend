#include "World.h"

World::World() {

}
World::~World() {

}

void World::createWorld(unsigned total = 100) {
	//srand(TIME(NULL));
	const int xMax(5000), yMax(5000);
	TOTAL = total;
	free = total;
	points.resize(TOTAL);
	dist.resize(TOTAL);
	graph.resize(TOTAL);
	int x, y;

	vector < vector < bool > > matrix(xMax + 5, vector<bool>(yMax + 5, false));

	x = rand() % 5000;
	y = rand() % 5000;

	for (int i = 0; i < TOTAL; i++) {
		dist[i].resize(TOTAL - 1);
		while (matrix[x][y]) {
			x = rand() % 5000;
			y = rand() % 5000;
		}
		for (int i = x - 2; i <= x + 2; i++) {
			for (int j = y - 2; j <= y + 2; j++) {
				matrix[i][j] = true;
			}
		}
		points[i].x = x, points[i].y = y, points[i].id = i;
	}
	getDistance();
	buildGraph();
}
void World::preparingCircuit(Point* const target, Point* const nextPoint, set<Point*>& circuit) {
	int lineID = target->connect;
	vector<int> way(TOTAL, -1);
	set<int> alreadyLook;
	list<int> queue;
	vector<Point*> temp = getNearestPoints(nextPoint);
	for (auto& i : temp) {
		if (i->id == target->id) continue;
		queue.push_back(i->id);
		way[i->id] = nextPoint->id;
	}
	alreadyLook.insert(nextPoint->id);

	while (!queue.empty()) {
		alreadyLook.insert(queue.front());
		for (auto i : graph[queue.front()]) {
			if (i == target->id && way[queue.front()] != i)
				way[target->id] = queue.front();
			if (points[i].connect != lineID || points[i].player != target->player)
				continue;
			if (alreadyLook.find(i) != alreadyLook.end()) continue;
			
			way[i] = queue.front();
			queue.push_back(i);

		}
		queue.pop_front();
		if (!queue.empty() && queue.front() == target->id) queue.pop_front();
	}
	int step = way[target->id];
	while (step != -1) {
		circuit.insert(&points[step]);
		step = way[step];
	}
	circuit.insert(target);

	for (int i = 0; i < TOTAL; i++) {
		if (points[i].connect == lineID && points[i].polygon == -1) {
			int count(0);
			for (auto point : graph[i]) {
				if (find(circuit.begin(), circuit.end(), &points[point]) != circuit.end())
					count++;
			}
			if (count > 1) circuit.insert(&points[i]);
		}
	}
}
void World::createPolygon(Point* const target, Point* const nextPoint) {
	set<Point*> circuit;
	preparingCircuit(target, nextPoint, circuit);

	vector<int> needUnite = unitePolygon(circuit);
	Polygon polygon;
	if (needUnite.size() == 1) this->polygons[needUnite[0]].wrap(circuit, graph, points);
	else {
		polygon._id = polygons.size();
		polygon._player = playerNow;
		polygons.push_back(polygon);
		polygons[polygons.size() - 1].wrap(circuit, graph, points);

	}

	if (needUnite.size() == 1) this->changeWorld(&polygons[needUnite[0]], needUnite);
	else this->changeWorld(&polygons[polygons.size()-1], needUnite);
}
vector<int> World::unitePolygon(set<Point*>& circuit) {
	map<int, int> neighborsPolygonID;
	vector<int> needUnite;
	for (auto& i : circuit)
		if (i->polygon != -1)
			neighborsPolygonID[i->polygon]++;
	for (auto& i : neighborsPolygonID) {
		if (i.second > 1) {
			needUnite.push_back(i.first);
			for (auto& j : polygons[i.first]._hull)
				circuit.insert(j);
		}
	}

	return needUnite;
}
void World::changeWorld(Polygon* polygon, vector<int>& needUnite) {
	if (needUnite.size() > 1) {
		for (auto& pID : needUnite) {
			unitePolygon(pID, polygon->_id);
		}
	}
	for (int i = 0; i < TOTAL; i++) {
		if (points[i].polygon == polygon->_id) {
			if (points[i].in_hull == false)
				points[i].is_inside = true;
			else continue;
		}
		if (polygon->is_inside(&points[i])) {
			if (points[i].player != polygon->_player && points[i].player != 0) {
				if (points[i].polygon != -1)
					deletePolygon(points[i].polygon);
				else
					deleteEdges(i), this->free++;

			}
			polygon->addPoint(&points[i]);
			this->createEdges(i);
			this->free--;
		}
	}
	cout << endl;
}
void World::createEdges(int newPointID) {
	vector<Point*> temp = getNearestPoints(&points[newPointID]);
	for (auto i : temp)
		(newPointID < i->id) ? edges.insert(std::make_pair(newPointID, i->id))
						   	 : edges.insert(std::make_pair(i->id, newPointID));
}
void World::unitePolygon(int deletePolygonID, int newPolygonID) {
	for (int i = 0; i < TOTAL; i++)
		if (points[i].polygon == deletePolygonID)
			points[i].polygon = newPolygonID, points[i].is_inside = true;
	polygons[deletePolygonID]._delete = true;
}
void World::deletePolygon(int deletePolygonID) {
	for (int i = 0; i < TOTAL; i++) {
		if (points[i].polygon == deletePolygonID) {
			this->free++;
			deleteEdges(i);
			points[i].polygon = -1;
			points[i].player = 0;
			points[i].connect = -1;
		}
	}
	polygons[deletePolygonID]._delete = true;
}
void World::deleteEdges(int deletePointID) {
	vector<Point*> temp = getNearestPoints(&points[deletePointID]);
	for (auto i : temp) {
		std::pair<int, int> edge = (deletePointID < i->id) ? std::make_pair(deletePointID, i->id)
														   : std::make_pair(i->id, deletePointID);
		edges.erase(edge);
	}
}

void World::updateGraph(const Point* one, const Point* two) {
	graph[one->id].push_back(two->id);
	graph[two->id].push_back(one->id);
}

void World::updateLineConnect(int oldId, int newId) {
	for (int i = 0; i < TOTAL; i++)
		if (points[i].connect == oldId)
			points[i].connect = newId;
}
void World::updatePointConnect(Point* point, int newId) {
	point->connect = newId;
}

int comp(pair<double, int>& a, pair<double, int>& b) {
	return a.first < b.first;
}
void World::getDistance() {
	double dx, dy;
	double avgDist = 0;
	double temp;
	for (int i = 0; i < TOTAL; i++) {
		for (int j = 0, k = 0; j < TOTAL; j++) {
			if (i == j) continue;
			dx = points[j].x - points[i].x;
			dy = points[j].y - points[i].y;
			temp = sqrt((dx * dx) + (dy * dy));
			dist[i][k].second = j;
			dist[i][k++].first = temp;
		}
		sort(dist[i].begin(), dist[i].end(), comp);
		temp = dist[i][TOTAL / 10].first;
		if (temp > avgDist) avgDist = temp;
	}
	capturDistance = avgDist/2;
//	capturDistance = static_cast<double>(avgDist / TOTAL);
}
void World::buildGraph() {
	for (int i = 0; i < TOTAL; i++) {
		int j = 0;
		while (j != TOTAL-1 && dist[i][j].first <= capturDistance)
			graph[i].push_back(dist[i][j++].second);
	}
}

vector<Point*> World::getNearestPoints(Point* point) {
	int id = point->id;
	int index;
	vector<Point*> neighbors;
	int i = 0;
	while (i != TOTAL-1 && dist[id][i].first <= capturDistance) {
		index = dist[id][i++].second;
		if (points[index].player == point->player)
			neighbors.push_back(&points[index]);
	}
	return neighbors;
}

bool World::haveFree() {
	return (free != 0);
}
