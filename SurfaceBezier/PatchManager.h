
#include <vector>
#include "Point.h"
#include "Bezier.h"

class PatchManager
{
	private:
		float nbRows;
		float nbPointsPerRow;
		std::vector< std::vector<Point> > allControlPoints;
		std::vector<Point> controlPoints;
		std::vector<Color> colors;
	public:
		PatchManager();
		PatchManager(float inbRows, float inbPointsPerRow);
		void generateControlPoints();
		void majControlPoints();
		void randomizeControlPoints();

		float getRandomNumber(int a, int b);
		std::vector<Point>& getControlPoints();
		std::vector<std::vector<Point>>& getAllControlPoints();
		std::vector<Color>& getColors();
		std::vector<Point> getBezierCurveOnRow(std::vector<Point> points);
}; 
