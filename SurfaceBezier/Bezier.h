#pragma once
#include <glm\glm.hpp>
#include <vector>
#include <iostream>

#define PI 3.1415926535

class Bezier
{
	private:
		std::vector<glm::vec3> controlPoints;

	public:
		int last2DCurvePointsCount;
		std::vector<std::vector<glm::vec3>> currentCurveObjects;

		std::vector<glm::vec3> GetControlPoints() const;
		std::vector<glm::vec3> Curve(float step, float min, float max, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
		glm::vec3 Casteljau(std::vector<glm::vec3> points, float step);
		std::vector<glm::vec3> CasteljauBezier(std::vector<glm::vec3> points, float step, std::vector<int> paramSpace);
		void Spline(std::vector<glm::vec3> points, std::vector<float> nodalVec, bool version1);
		//Raccordements avec des niveau allant de 0 à 2 C0, C1, C2
		std::vector<glm::vec3> Bezier::Raccord(int level, std::vector<glm::vec3> points, std::vector<int> paramSpace);

		//Extrusions functions
		std::vector<glm::vec3> simpleExtrude(std::vector<glm::vec3> points, float length, float step, float scale);
		std::vector<glm::vec3> revolutionExtrude(std::vector<glm::vec3> points, float step, float radius);
		std::vector<glm::vec3> generalExtrude(std::vector<glm::vec3> points, std::vector<glm::vec3> linePoints, float step);

		std::vector<glm::vec3> getFirstPointsFromSimpleExtrude(std::vector<glm::vec3> points, float length, float step, float scale);
		std::vector<glm::vec3> getLastPointsFromSimpleExtrude(std::vector<glm::vec3> points, float length, float step, float scale);

		std::vector<glm::vec3> getFirstPointsFromRevolutionExtrude(std::vector<glm::vec3> points, float step);
		std::vector<glm::vec3> getLastPointsFromRevolutionExtrude(std::vector<glm::vec3> points, float step);

		glm::vec3 getBarycenter(std::vector<glm::vec3> points);
};