
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
		std::vector<Point> casteljauPoints;
		std::vector<Color> colors;

		Bezier bezierManager;

	public:
		PatchManager();
		PatchManager(float inbRows, float inbPointsPerRow);
		void generateControlPoints();
		void majControlPoints();
		void randomizeControlPoints();

		float getRandomNumber(int a, int b);
		std::vector<Point>& getCasteljauPoints();
		std::vector<Point>& getControlPoints();
		std::vector<std::vector<Point>>& getAllControlPoints();
		std::vector<Color>& getColors();
		void getBezierCurveOnRow(std::vector<Point> controlPoints, int steps, std::vector<int> parameterSpace);
}; 
