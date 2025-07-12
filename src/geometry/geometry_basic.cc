#include <iostream>
#include <iomanip>
#include <cmath>
#include <array>
#include <algorithm>
#include <cassert>
#include <vector>
#include <stack>

using Real = long double;

constexpr Real EPS = 1e-10;
const Real PI = acos(static_cast<Real>(-1.0)); // GCC 4.6.1 以上で acos() は constexpr の場合がある

inline int sign(Real a) { return (a < -EPS) ? -1 : (a > EPS) ? +1 : 0; }
inline bool eq(Real a, Real b)  { return sign(a - b) == 0; }  // a = b
inline bool neq(Real a, Real b) { return !eq(a, b); }         // a != b
inline bool lt(Real a, Real b)  { return sign(a - b) == -1; } // a < b
inline bool leq(Real a, Real b) { return sign(a - b) <= 0; }  // a <= b
inline bool gt(Real a, Real b)  { return sign(a - b) == 1; }  // a > b
inline bool geq(Real a, Real b) { return sign(a - b) >= 0; }  // a >= b

// change between degree and radian
inline Real to_radian(const Real degree) { return degree * (PI / 180.0); }
inline Real to_degree(const Real radian) { return radian * 180.0 / PI; }

/**
 * Point in two dimensional
 */
struct Point2 {
    Real x{0.0}, y{0.0};

    explicit Point2() {}
    Point2(Real x, Real y) : x(x), y(y) {}

    // Arithmetic operator between Point2s
    Point2 operator+(const Point2 &rhs) const { return Point2(x + rhs.x, y + rhs.y); }
    Point2 operator-(const Point2 &rhs) const { return Point2(x - rhs.x, y - rhs.y); }
    Point2 operator*(const Point2 &rhs) const { // cross product between Point2s
        return Point2(x * rhs.x - y * rhs.y, x * rhs.y + y * rhs.x);
    }

    // Unary operator and compound assignment operator
    Point2 operator-() const { return {-x, -y}; }
    Point2& operator+=(const Point2 &rhs) { return *this = *this + rhs; }
    Point2& operator-=(const Point2 &rhs) { return *this = *this - rhs; }

    // Arithmetic operator between Point2 and Real
    Point2 operator*(Real rhs) const { return Point2(x * rhs, y * rhs); }
    Point2 operator/(Real rhs) const { return Point2(x / rhs, y / rhs); }

    // Comparison operation
    bool operator==(const Point2 &rhs) const { return eq(x, rhs.x) && eq(y, rhs.y); }
    bool operator!=(const Point2 &rhs) const { return neq(x, rhs.x) || neq(y, rhs.y); }
    bool operator<(const Point2 &rhs) const { return lt(x, rhs.x) || (eq(x, rhs.x) && lt(y, rhs.y)); }
    bool operator>(const Point2 &rhs) const { return gt(x, rhs.x) || (eq(x, rhs.x) && gt(y, rhs.y)); }

    // Other operator
    // ユークリッド距離を返す
    Real abs(void) const { return std::hypot(x, y); }

    // ユークリッド距離の二乗を返す
    Real abs2(void) const { return x * x + y * y; }

    // 単位はラジアンで範囲 [-PI, PI] で x 軸の正の方向となす角度を返す
    // atan2(y, x) は y / x の逆正接を返す（arctan(y / x)）
    // atan(z) と異なりどの象限に属しているか分かるので正しい符号を返す
    Real arg(void) const { return atan2(y, x); }

    // 内積
    Real dot(const Point2 &rhs) const { return x * rhs.x + y * rhs.y; }

    // 原点を中心に反時計回りに90度回転する
    Point2 rotate90(void) { return *this = Point2(-y, x); }

    // 原点を中心に反時計回りに angle [rad] だけ回転する
    void rotate(Real angle) {
            *this = Point2(cos(angle) * x - sin(angle) * y, sin(angle) * x + cos(angle) * y);
    }
};

Point2 operator*(Real a, Point2 p) { return p * a; }

// Output and input of a Point2
std::ostream& operator<<(std::ostream &os, const Point2 &p) { return os << p.x << ' ' << p.y; }
std::istream& operator>>(std::istream &is, Point2 &p) { return is >> p.x >> p.y; }

// ベクトル p1 と p2 の内積： dot(p1, p2) = |a| |b| cos(theta)
inline Real dot(const Point2 &p1, const Point2 &p2) { return p1.x * p2.x + p1.y * p2.y; }

// ベクトル p1 と p2 の外積の絶対値 |p1 x p2| ： |p1 x p2| = |p1| |p2| sin(theta)
// 原点, p1, p2 を頂点とする符号付き平行四辺形の面積（ p1 から p2 へ反時計回りで符号が正）
inline Real abs_cross(const Point2 &p1, const Point2 &p2) { return p1.x * p2.y - p1.y * p2.x; }

// ベクトル p1 から p2 への角度を返す（単位はラジアン）
// p1 と p2 のなす角度で小さい方で p1 から p2 へ反時計回りなら符号は正，時計回りなら符号は負
inline Real arg(const Point2 &p1, const Point2 &p2) { return atan2(abs_cross(p1, p2), dot(p1, p2)); }

// Whether one object contains the other
enum class CONTAIN {
    IN,
    ON,
    OUT,
};

// Counter-Clockwise predicate (a, b, c)
enum class CCW : int {
    COUNTER_CLOCKWISE = 1,     // counter clockwise
    CLOCKWISE         = -1,    // clockwise
    ONLINE_FRONT      = 2,     // a--b--c on line or (a == b and b != c)
    ONLINE_BACK       = -2,    // c--a--b on line
    ON_SEGMENT        = 0,     // a--c--b on line or (a != b and b == c) or (a == b == c)
    OTHER             = -3,
};

// dir と逆の向きを返す
CCW inv(const CCW dir) {
    switch (dir) {
        case (CCW::COUNTER_CLOCKWISE): return CCW::CLOCKWISE;
        case (CCW::CLOCKWISE): return CCW::COUNTER_CLOCKWISE;
        case (CCW::ONLINE_FRONT): return CCW::ONLINE_BACK;
        case (CCW::ONLINE_BACK): return CCW::ONLINE_FRONT;
        default: return dir;
    }
}

// Counter-Clockwise predicaste (a, b, c)
CCW ccw(const Point2 &a, Point2 b, Point2 c) {
    b -= a;  c -= a;
    if (sign(abs_cross(b, c)) == 1) return CCW::COUNTER_CLOCKWISE;
    if (sign(abs_cross(b, c)) == -1) return CCW::CLOCKWISE;
    if (sign(dot(b, c)) == -1)       return CCW::ONLINE_BACK;
    if (sign(b.abs2() - c.abs2()) == -1)   return CCW::ONLINE_FRONT;
    return CCW::ON_SEGMENT;
}

// ccw 関数の戻り値を CCW の規定型（int 型）で返す
auto ccw_t(const Point2 &a, Point2 b, Point2 c) {
    return static_cast<std::underlying_type<CCW>::type>(ccw(a, std::move(b), std::move(c)));
}


/**
 * Line in two dimensional
 */
class Line : public std::array<Point2, 2> {
public:
    explicit Line() {}
    Line(const Point2 &p1, const Point2 &p2) {
        (*this)[0] = p1;
        (*this)[1] = p2;
    }
};

// Input and output of a Line
std::istream& operator>>(std::istream &is, Line &l) { return is >> l[0] >> l[1]; }
std::ostream& operator<<(std::ostream &os, const Line &l) { return os << l[0] << ' ' << l[1]; }

CCW ccw(const Line &l, const Point2 &p) { return ccw(l[0], l[1], p); }
auto ccw_t(const Line &l, const Point2 &p) { return ccw_t(l[0], l[1], p); }



/**
 * Segment in two dimensional
 */
class Segment : public Line {
public:
    explicit Segment() {}
    Segment(const Point2 &p1, const Point2 &p2) : Line(p1, p2) {}
};


/**
 * Circle in two dimensional
 */
class Circle : public Point2 {
public:
    Real r;
    explicit Circle() {}
    Circle(const Point2 &p, Real r = 0.0) : Point2(p), r(r) {}
    Circle& operator=(const Point2 &p) {
        this->x = p.x; this->y = p.y;
        return *this; 
    }

    Real area() const { return PI * r * r; }
    Real area_circular_sector(const Point2 &p1, const Point2 &p2, const bool strict_check = false) const; // 扇形の面積
    Real area_circular_segment(const Point2 &p1, const Point2 &p2, const bool strict_check = false) const; // 弓形の面積

    // Is p contained or on segment or otherwise? : O(n)
    CONTAIN contain(const Point2 &p) const;
    bool contain(const Circle &rhs) const;
};

// Input of a circle
std::istream& operator>>(std::istream &is, Circle &c) { return is >> c.x >> c.y >> c.r; }
std::ostream& operator<<(std::ostream &os, const Circle &c) { return os << c.x << ' ' << c.y << ' ' << c.r; }



/**
 * Intersection testing
 */
Point2 projection(const Line &l, const Point2 &p) {
    Point2 dir = l[1] - l[0];
    Real t = dot(p - l[0], dir) / dir.abs2();
    return l[0] + dir * t;
}

inline Point2 reflection(const Line &l, const Point2 &p) {
    return projection(l, p) * 2.0 - p;
}

inline bool is_orthogonal(const Line &l1, const Line &l2) {
    return eq(dot(l1[0] - l1[1], l2[0] - l2[1]), 0.0);
}

inline bool is_parallel(const Line &l1, const Line &l2) {
    return eq(abs_cross(l1[0] - l1[1], l2[0] - l2[1]), 0.0);
}

inline bool is_intersect(const Line &l, const Point2 &p) {
    return std::abs(ccw_t(l[0], l[1], p)) != 1;
}

inline bool is_intersect(const Segment &s, const Point2 &p) {
    return ccw(s[0], s[1], p) == CCW::ON_SEGMENT;
}

inline bool is_intersect(const Line &l1, const Line &l2) {
    return !is_parallel(l1, l2) || is_parallel(l1, Line(l1[0], l2[0]));
}

inline bool is_intersect(const Line &l, const Segment &s) {
    return sign(abs_cross(l[1] - l[0], s[0] - l[0]) *
                abs_cross(l[1] - l[0], s[1] - l[0])) <= 0;
}

inline bool is_intersect(const Segment &s1, const Segment &s2) {
    return ccw_t(s1[0], s1[1], s2[0]) * ccw_t(s1[0], s1[1], s2[1]) <= 0 &&
        ccw_t(s2[0], s2[1], s1[0]) * ccw_t(s2[0], s2[1], s1[1]) <= 0;
}

inline bool is_intersect(const Circle &c, const Point2 &p) { // p is in interior or boundary
    return leq((c - p).abs(), c.r);
}

inline bool is_intersect(const Circle &c, const Segment &s) {
    return is_intersect(c, s[0]) || is_intersect(c, s[1]) ||
        (is_intersect(c, projection(s, c))
         && ccw(s[0], projection(s, c), s[1]) == CCW::ONLINE_FRONT);
}

inline bool is_intersect(const Circle &c, const Line &l) {
    return is_intersect(c, projection(l, c));
}

// 2円が内接・2点で交わる・外接するなら true、含まれる・離れいているなら false
inline bool is_intersect(const Circle &c1, const Circle &c2) {
    return sign(c1.r + c2.r - (c1 - c2).abs()) >= 0 &&
        sign((c1 - c2).abs() - std::abs(c1.r - c2.r) >= 0);
}


/**
 * Distance and Intersection Point2
 */
inline Real distance(const Point2 &p1, const Point2 &p2) {
    return (p1 - p2).abs();
}

inline Real distance(const Line &l, const Point2 &p) {
    return (p - projection(l, p)).abs();
}

inline Real distance(const Segment &s, const Point2 &p) {
    if (sign(dot(s[1] - s[0], p - s[0])) == -1) return (p - s[0]).abs();
    if (sign(dot(s[0] - s[1], p - s[1])) == -1) return (p - s[1]).abs();
    return (p - projection(s, p)).abs();
}

inline Real distance(const Line &l1, const Line &l2) {
    return is_intersect(l1, l2) ? 0 : distance(l1, l2[0]);
}

inline Real distance(const Line &l, const Segment &s) {
    if (is_intersect(l, s)) return 0.0;
    return std::min(distance(l, s[0]), distance(l, s[1]));
}

inline Real distance(const Segment &s1, const Segment &s2) {
    if (is_intersect(s1, s2)) return 0.0;
    return std::min({distance(s1, s2[0]), distance(s1, s2[1]),
                distance(s2, s1[0]), distance(s2, s1[1])});
}

Point2 cross_point(const Line &l1, const Line &l2) {
    Real A = abs_cross(l1[1] - l1[0], l2[1] - l2[0]);
    Real B = abs_cross(l1[1] - l1[0], l1[1] - l2[0]);
    if (sign(std::abs(A)) == -1 && sign(std::abs(B)) == -1) return l2[0];
    if (sign(std::abs(A)) == -1) assert(false);
    return l2[0] + (l2[1] - l2[0]) * B / A;
}

std::vector<Point2> cross_point(const Circle &c, const Line &l) {
    if (!is_intersect(c, l)) return std::vector<Point2>();
    Point2 mid = projection(l, c);
    if (eq((c - mid).abs(), c.r)) return {mid};

    Point2 e = (l[1] - l[0]) / (l[1] - l[0]).abs();
    Real len = std::sqrt(c.r * c.r - (mid - c).abs2());
    return {mid + e * len, mid - e * len};
}

// 円 c と線分 s の交点を求める（s の端点も含む可能性がある）
std::vector<Point2> cross_point(const Circle &c, const Segment &s) {
    if (!is_intersect(c, s)) return std::vector<Point2>();

    const Point2 mid = projection(s, c), e = (s[1] - s[0]) / (s[1] - s[0]).abs();
    if (eq(c.r, (mid - c).abs())) return { mid };

    const Real len = std::sqrt(c.r * c.r - (mid - c).abs2());
    const Point2 p1 = mid + e * len, p2 = mid - e * len;
    const CCW ccw1 = ccw(s[0], p1, s[1]); 

    if (p1 == p2 && ccw1 == CCW::ONLINE_FRONT) return {p1};

    const CCW ccw2 = ccw(s[0], p2, s[1]);
    std::vector<Point2> ps;
    if (ccw1 == CCW::ONLINE_FRONT || p1 == s[1]) ps.emplace_back(p1);
    if (ccw2 == CCW::ONLINE_FRONT || p2 == s[1]) ps.emplace_back(p2);

    if (ps.size() == 2 && ccw(s[0], ps.back(), ps.front()) == CCW::ONLINE_FRONT)
        std::swap(ps.front(), ps.back());
    return ps;
}

std::vector<Point2> cross_point(const Circle &c1, const Circle &c2) {
    if (!is_intersect(c1, c2))
        return std::vector<Point2>();
    Real d = distance(c1, c2);

    // Herbie による提案: Real r1_cos = (d * d + c1.r * c1.r - c2.r * c2.r) / (2.0 * d);
    Real r1_cos = 0.5 * (d + ((c1.r + c2.r) / d) * (c1.r - c2.r));
    Real h = std::sqrt(c1.r * c1.r - r1_cos * r1_cos);
    Point2 base = c1 + (c2 - c1) * r1_cos / d;
    Point2 dir = (c2 - c1).rotate90() * h / d;
    if (dir == Point2(0, 0))
        return {base};
    return {base + dir, base - dir};
}

// the tangent line from a Point2 to a circle
std::vector<Point2> tangent_point(const Circle &c, const Point2 &p) {
    Real x = (p - c).abs2();
    Real d = x - c.r * c.r;
    if (sign(d) == -1) // no Point2
        return std::vector<Point2>();
    d = std::max(d, (Real)0.0);
    Point2 q1 = (p - c) * (c.r * c.r / x);
    Point2 q2 = ((p - c) * (-c.r * std::sqrt(d) / x)).rotate90();
    if (q2 == Point2(0, 0)) return {c + q1};
    return {c + q1 - q2, c + q1 + q2};
}

// common tangent lines to two circles
std::vector<Line> common_tangent(const Circle &c1, const Circle &c2) {
    // two circle contact one Point2 internally
    if (eq(distance(c1, c2), std::abs(c1.r - c2.r))) { // |  $ $|
        Point2 cross_Point2 = cross_point(c1, c2)[0];
        Point2 up = (cross_Point2 - c1).rotate90();
        return {Line(cross_Point2 + up, cross_Point2 - up)};
    }

    std::vector<Line> list;

    // caluculate outer tangent
    if (eq(c1.r, c2.r)) {
        Point2 dir = c2 - c1;
        dir = (dir * (c1.r / dir.abs())).rotate90();
        list.emplace_back(Line(c1 + dir, c2 + dir));
        list.emplace_back(Line(c1 - dir, c2 - dir));
    }
    else {
        Point2 p = (c1 * (-c2.r)) + (c2 * c1.r);
        p = p * (1 / (c1.r - c2.r));
        std::vector<Point2> ps = tangent_point(c1, p);
        std::vector<Point2> qs = tangent_point(c2, p);
        const int N = std::min(ps.size(), qs.size());
        for (int i = 0; i < N; ++i)
            list.emplace_back(Line(ps[i], qs[i]));
    }

    // caluculate inner tangent
    if (eq(distance(c1, c2), c1.r + c2.r)) {
        // two circle contact one Point2 outernally | |$ $
        Point2 cross_Point2 = cross_point(c1, c2)[0];
        Point2 up = (cross_Point2 - c1).rotate90();
        list.emplace_back(Line(cross_Point2 + up, cross_Point2 - up));
    }
    else { // | |  $ $
        Point2 p = (c1 * c2.r) + (c2 * c1.r);
        p = p * (1 / (c1.r + c2.r));
        std::vector<Point2> ps = tangent_point(c1, p);
        std::vector<Point2> qs = tangent_point(c2, p);
        const int N = std::min(ps.size(), qs.size());
        for (int i = 0; i < N; ++i)
            list.emplace_back(Line(ps[i], qs[i]));
    }

    return list;
}

Real Circle::area_circular_sector(const Point2 &p1, const Point2 &p2, const bool strict_check) const {
    // p1 または p2 が円周上の点ではない場合（数値誤差のため corss_point 関数で求めた点が円周上の点ではない場合がある）
    if (strict_check) {
        if (neq(r, distance(*this, p1)) || neq(r, distance(*this, p2))) return 0.0;
    }
    if (p1 == p2) return 0.0;
    return 0.5 * r * r * std::abs(::arg(p1 - *this, p2 - *this));
}

Real Circle::area_circular_segment(const Point2 &p1, const Point2 &p2, const bool strict_check) const {
        // p1 または p2 が円周上の点ではない場合（数値誤差のため corss_point 関数で求めた点が円周上の点ではない場合がある）
    if (strict_check) {
        if (neq(r, distance(*this, p1)) || neq(r, distance(*this, p2))) return 0.0;
    }
    Real area = this->area_circular_sector(p1, p2);
    if (eq(area, 0.0)) return 0.0;
    return area - 0.5 * std::abs(abs_cross(p1 - *this, p2 - *this));
}

CONTAIN Circle::contain(const Point2 &p) const {
    const Real d = distance(p, p);
    return eq(this->r, d) ? CONTAIN::ON : (lt(this->r, d) ? CONTAIN::OUT : CONTAIN::IN);
}

// this が rhs を含む・内接するなら true、それ以外（2点で交わる・外接する・離れている）なら false を返す
bool Circle::contain(const Circle &rhs) const {
    return leq(rhs.r, this->r) && leq((*this - rhs).abs(), std::abs(this->r - rhs.r));
}


/**
 * Polygon: The order of the points in the polygon is counter-clockwise.
 */
class Polygon {
public:
    explicit Polygon() {}
    explicit Polygon(int size) : points(size){}
    explicit Polygon(std::initializer_list<Point2> p) : points(p.begin(), p.end()) {}
    // explicit Polygon(std::vector<Point2> p) : points(std::move(p)) {}
    explicit Polygon(std::vector<Point2> &&p) : points(p) {}

    Real area() const; // area of polygon : O(n)
    bool is_convex() const; // Test whether it's convex polygon : O(n)
    Polygon convex_cut(const Line &l) const; // cut the polygon by line and return the left: O(n)

    // Is p contained or on segment or otherwise? : O(n)
    CONTAIN contain(const Point2 &p) const;
    // convex version : O(log n)
    CONTAIN convex_contain(const Point2 &p) const;

    std::vector<Point2> points;
};

// Output of a polygon
std::ostream& operator<<(std::ostream &os, const Polygon &poly) {
    for (auto p : poly.points) os << p << ", ";
    return os;
}

Real Polygon::area() const {
    const int n = points.size();
    assert(1 < n);

    Real area = abs_cross(points[n - 1], points[0]);
    for (int i = 0; i < n - 1; ++i)
        area += abs_cross(points[i], points[i + 1]);
    return 0.5 * area;
}

bool Polygon::is_convex() const {
    // if given polygon is not simple polygon we should check for all (i-1, i, i+1)
    // (p[i].y <= p[i-1].y && p[i].y < p[i+1].y)
    // doesn't happen  without first index which y is the lowest
    const int n = points.size();
    CCW diff = CCW::OTHER;
    for (int i = 0; i < n; ++i) {
        CCW cur = ccw(points[i], points[(i + 1) % n], points[(i + 2) % n]);
        if (diff == CCW::OTHER && (cur == CCW::CLOCKWISE || cur == CCW::COUNTER_CLOCKWISE))
            diff = inv(cur);
        else if (cur == diff) return false;
    }
    return true;
}

CONTAIN Polygon::contain(const Point2 &p) const {
    const int n = points.size();
    bool count = false;
    for (int i = 0; i < n; ++i) {
        if (is_intersect(Segment(points[i], points[(i + 1) % n]), p))
            return CONTAIN::ON;
        Point2 up = points[i] - p, down = points[(i + 1) % n] - p;
        if (up.y < down.y)
            std::swap(up, down);
        if (sign(down.y) <= 0 && sign(up.y) == 1 && sign(abs_cross(up, down)) == 1)
            count = !count;
    }
    return count ? CONTAIN::IN : CONTAIN::OUT;
}

CONTAIN Polygon::convex_contain(const Point2 &p) const {
    const int n = points.size();
    Point2 g = (points[0] + points[n / 3] + points[2 * n / 3]) / 3.0; // inner Point2
    int a = 0, b = n;

    while (a + 1 < b) { // invariant : c is in fan g-poly[a]-poly[b]
        int c = (a + b) * 0.5;
        if (sign(abs_cross(points[a] - g, points[c] - g)) == 1) { // angle < 180 deg
            if (sign(abs_cross(points[a] - g, p - g)) >= 0 &&
                sign(abs_cross(points[c] - g, p - g)) == -1)
                b = c;
            else
                a = c;
        }
        else {
            if (sign(abs_cross(points[a] - g, p - g)) == -1 &&
                sign(abs_cross(points[c] - g, p - g)) == 1)
                a = c;
            else
                b = c;
        }
    }

    // Assume that Point2s in polygon are in the order of counter-clockwise
    b %= n;
    int res = sign(abs_cross(points[a] - p, points[b] - p));
    return (res == -1 ? CONTAIN::OUT : (res == 1 ? CONTAIN::IN : CONTAIN::ON));
}

// クイックハル法： 凸法上の頂点とはならない頂点を除外する前処理を O(n m) 時間で行う
// （360度を m 等分した角度を傾きに持つ m 本の直線と接する点からなる多角形に含まれない点を前もって除外する）
// Andrew's Algorithm では凸法上の頂点数が少なくてもソートの計算時間が寄与するので O(n log n) となる
void shrink_to_points(std::vector<Point2> &ps, int m = 4) {
    std::vector<Point2> sep(m, ps.front());

    Point2 dir(0, -1);
    for (auto &sep_i : sep) {
        for (const auto &p : ps)
            if (sep_i.dot(dir) < p.dot(dir)) sep_i = p;
        dir.rotate(2.0 * PI / m);
    }
    sep.erase(std::unique(sep.begin(), sep.end()), sep.end());

    int add_idx = ps.size() - 1;
    for (int i = ps.size() - 1; 0 <= i; --i) {
        bool is_contain = true;
        for (int j = 0; j < (int)sep.size(); ++j) {
            if (ccw(sep[j], sep[(j + 1) % 4], ps[i]) != CCW::COUNTER_CLOCKWISE) {
                is_contain = false;
                break;
            }
        }
        if (is_contain) std::swap(ps[i], ps[add_idx--]);
    }
    ps.resize(add_idx + 1);
}

// Andrew's Monotone Chain Algorithm : O(n * log n)
Polygon convex_hull(std::vector<Point2> ps) {
    if (4 < ps.size()) shrink_to_points(ps);
    if (ps.size() < 3) return Polygon(std::move(ps));

    const int n = ps.size();
    int size = 0;
    std::vector<Point2> chain(2 * n);

    std::sort(ps.begin(), ps.end());

    for (int i = 0; i < n; chain[size++] = ps[i++])  // lower hull
        while (size >= 2 && ccw_t(chain[size - 2], chain[size - 1], ps[i]) <= 0)
            --size;
    for (int i = n - 2, j = size + 1; 0 <= i; chain[size++] = ps[i--])  // upper hull
        while (size >= j && ccw_t(chain[size - 2], chain[size - 1], ps[i]) <= 0)
            --size;
    chain.resize(size - 1);

    return Polygon(std::move(chain));
}

// rotating calipers algorithm : O(n)
Real convex_diameter(const std::vector<Point2> &poly) {
    const int n = poly.size();

    std::pair<int, int> s; // first is min, second is max
    for (int i = 1; i < n; ++i) {
        if (poly[i].y < poly[s.first].y) s.first = i;
        if (poly[i].y > poly[s.second].y) s.second = i;
    }

    Real max_d = distance(poly[s.first], poly[s.second]);
    std::pair<int, int> cur(s), max_p(s);

    // Assume that Point2s in polygon are in the order of counter-clockwise
    do {
        Point2 v1 = poly[cur.second] - poly[(cur.second + 1) % n] + poly[cur.first];
        if (ccw(poly[cur.first], poly[(cur.first + 1) % n], v1) == CCW::COUNTER_CLOCKWISE)
            cur.first = (cur.first + 1) % n;
        else
            cur.second = (cur.second + 1) % n;

        Real cur_d = distance(poly[cur.first], poly[cur.second]);
        if (max_d < cur_d) {
            max_p = cur;
            max_d = cur_d;
        }
    } while (cur != s);

    return max_d;
}

Polygon Polygon::convex_cut(const Line &l) const {
    const int n = points.size();
    Polygon q; // left side polygon cutted by line

    for (int i = 0; i < n; ++i) {
        Point2 cur(points[i]), next(points[(i + 1) % n]);
        if (ccw(l, cur) != CCW::CLOCKWISE)
            q.points.emplace_back(cur);
        if (ccw_t(l, cur) * ccw_t(l, next) == -1)
            q.points.emplace_back(cross_point(l, Line(cur, next)));
    }

    return q;
}

// 円板 c と多角形 poly の共通部分の面積を返す： O(n) 時間
// 多角形の面積を隣接する二点間の符号付き面積で求める方法を用いる
Real area_intersection(const Circle &c, const Polygon &poly) {
    Real area = 0.0;

    const int n = poly.points.size();
    for (int i = 0; i < n; ++i) {
        const Point2 &p1 = poly.points[i] - c, &p2 = poly.points[(i + 1) % n] - c;

        // p1 と p2 が同一直線上にある場合は面積は 0 なのでスキップ
        if (abs(ccw_t(c, p1, p2)) != 1) continue;

        if (lt(p1.abs(), c.r) && lt(p2.abs(), c.r)) {
            area += 0.5 * abs_cross(p1, p2);
        }
        else if (lt(p1.abs(), c.r)) {
            const std::vector<Point2> ps = cross_point(c, Segment(p1, p2));
            area += 0.5 * abs_cross(p1, ps.front());
            area += 0.5 * c.r * c.r * arg(ps.front(), p2);
        }
        else if (lt(p2.abs(), c.r)) {
            const std::vector<Point2> ps = cross_point(c, Segment(p1, p2));
            area += 0.5 * c.r * c.r * arg(p1, ps.front());
            area += 0.5 * abs_cross(ps.front(), p2);
        }
        else {
            const std::vector<Point2> ps = cross_point(c, Segment(p1, p2));
            if (ps.size() == 0) area += 0.5 * c.r * c.r * arg(p1, p2);
            else {
                area += 0.5 * c.r * c.r * arg(p1, ps.front());
                area += 0.5 * abs_cross(ps.front(), ps.back());
                area += 0.5 * c.r * c.r * arg(ps.back(), p2);
            }
        }
    }

    return area;
}

// --------------------8<------- start of main part of library -------8<--------------------
// --------------------8<------- end of main part of library   -------8<--------------------


int main() {
    std::cout << std::fixed << std::setprecision(10);
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    Point2 p{1, 2}, q{2, 3};
    std::cout << p * 2 << std::endl;
    std::cout << 2 * p << std::endl;
    std::cout << p + q << std::endl;

    Polygon poly({Point2(0.0, 0.0), Point2(10.0, 0.0), Point2(10.0, 10.0), Point2(0.0, 10.0)});
    std::cout << poly.area() << std::endl;

    return 0;
}
