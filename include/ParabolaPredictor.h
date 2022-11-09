#pragma once

#include "GlobalIncludes.h"

class ParabolaPredictor {
public:
    struct Parabola {
        float a = 0, b = 0, c = 0;

        static Parabola from2Points(Vector2 p1, Vector2 p2, float a) {
            Vector2 col1 = {p1.x(), p2.x()};
            Vector2 col2 = {1, 1};
            Matrix2x2 A = Matrix2x2{col1, col2};

            Vector2 C = {p1.y() - a * p1.x() * p1.x(),
                         p2.y() - a * p2.x() * p2.x()};

            Vector2 B = A.inverted() * C;

            return Parabola{a, B.x(), B.y()};
        }

        static Parabola from3Points(Vector2 p1, Vector2 p2, Vector2 p3) {
            Vector3 col1 = {p1.x() * p1.x(), p2.x() * p2.x(), p3.x() * p3.x()};
            Vector3 col2 = {p1.x(), p2.x(), p3.x()};
            Vector3 col3 = {1, 1, 1};
            Matrix3 A = Matrix3{col1, col2, col3};

            Vector3 C = {p1.y(), p2.y(), p3.y()};

            Vector3 B = A.inverted() * C;

            return Parabola{B.x(), B.y(), B.z()};
        }

        float derivative(float t) const {
            return 2 * a * t + b;
        }

        float derivative2() const {
            return 2 * a;
        }

        float operator()(float t) const {
            return t * t * a / 2 + t * b + c;
        }
    };

    struct Prediction {
        Parabola parabola;
        Vector2 start, speed;

        Vector3 operator()(float t) const {
            return {start.x() + speed.x() * t, parabola(t), start.y() + speed.y() * t};
        }
    };

private:
    const float _g = -9.807;

    map<float, Vector3> _points;
    Prediction _prediction;

    bool generate() {
        vector<Vector3> points;
        vector<float> times;

        for (auto [t, p]: _points) {
            times.push_back(t);
            points.push_back(p);
        }

        if (_points.size() < 3)
            return false;

        float sumOffLine = 0;
        float sumOffPar = 0;

        for (int i = 0; i < int(points.size() - 2); i++) {
            auto par = Parabola::from3Points(
                    {times[i], points[i].y()},
                    {times[i + 1], points[i + 1].y()},
                    {times[i + 2], points[i + 2].y()});

            float offPar = abs(par.derivative2() - _g);

            if (offPar >= 0.1f)
                return false; // second derivative of heights is too far off, may also be measurement error though
            sumOffPar += offPar;

            Vector2 a = Vector2{points[i + 1].x(), points[i + 1].z()} - Vector2{points[i].x(), points[i].z()};
            Vector2 b = Vector2{points[i + 2].x(), points[i + 2].z()} - Vector2{points[i + 1].x(), points[i + 1].z()};

            float offLine = abs((a * b).dot() / a.length() / b.length());

            if (offLine < cos(Deg(10)))
                return false; // points aren't really on a line, may also be measurement error though

            sumOffLine += offLine;
        }

        if (sumOffPar / float(points.size()-2) > 0.05f)
            return false; // second derivative of heights is too far off, probably isn't movement influenced by gravity

        if (sumOffLine / float(points.size()-2) < cos(Deg(5)))
            return false; // points aren't really on a line, probably isn't flying through the air


        Prediction newPrediction;
        newPrediction.speed = Vector2{points.back().x() - points[0].x(), points.back().z() - points[0].z()} /
                (times.back() - times[0]);

        newPrediction.start = Vector2{points[0].x(), points[0].z()} + newPrediction.speed * (-times[0]);

        float averageB = 0;
        float averageC = 0;

        for (int i = 0; i < int(points.size() - 1); i++) {
            auto par = Parabola::from2Points({times[i], points[i].y()}, {times[i + 1], points[i + 1].y()}, _g / 2);
            averageB += par.b;
            averageC += par.c;
        }

        newPrediction.parabola = {_g / 2, averageB / float(points.size()), averageC / float(points.size())};

        _prediction = newPrediction;
        return true;
    }

public:
    Vector3 operator()(float t) const {
        return _prediction(t);
    }

    bool addPoint(float t, Vector3 p) {
        assert(_points.count(t) == 0);
        _points[t] = p;

        return generate();
    }

    void clear() {
        _points.clear();
        _prediction = {};
    }

};


