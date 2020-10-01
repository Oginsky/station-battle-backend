#pragma once

struct Point {
	int x, y;
	int id = -1;
	int player = 0;
	int connect = -1;
	int polygon = -1;
	bool is_inside = false;
	bool in_hull = false;

	Point(int x, int y, int id) { this->x = x, this->y = y, this->id = id; }
	Point() {}
};

static double getAngle(Point* center, Point* point) {
	int x = point->x - center->x;
	int y = point->y - center->y;
	if (x == 0.0) return ((y > 0.0) ? 90 : 270);
	double theta = atan((double)y / (double)x); // в радианах
	theta *= 360 / (2 * 3.1415926); // перевод в градусы
	if (x > 0.0) return ((y >= 0.0) ? theta : 360 + theta); // 1 и 4 квадранты
	else return (180 + theta); // 2 и З квадранты
}

static double oriented_tri_area(Point a, Point b, Point c) {
	return double(b.x - a.x) * (c.y - a.y) - double(b.y - a.y) * (c.x - a.x);
}
static bool projection_intersection(int a, int b, int c, int d) {
	if (a > b) std::swap(a, b);
	if (c > d) std::swap(c, d);
	return std::max(a, c) <= std::min(b, d);
}
static bool edges_intersect(std::pair<Point, Point> a, std::pair<Point, Point> b) {
	bool answer = (projection_intersection(a.first.x, a.second.x, b.first.x, b.second.x)
		&& projection_intersection(a.first.y, a.second.y, b.first.y, b.second.y)
		&& oriented_tri_area(a.first, a.second, b.first) * oriented_tri_area(a.first, a.second, b.second) <= 0
		&& oriented_tri_area(b.first, b.second, a.first) * oriented_tri_area(b.first, b.second, a.second) <= 0);
	return answer;
}