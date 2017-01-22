
#include <vector>
#include "Point.h"

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

		std::vector<Point>& getControlPoints();
		std::vector<std::vector<Point>>& getAllControlPoints();
		std::vector<Color>& getColors();
}; 
