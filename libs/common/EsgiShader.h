// ---------------------------------------------------------------------------
//
// ESGI OpenGL (ES) 2.0 Framework
// Malek Bengougam, 2012							malek.bengougam@gmail.com
//
// ---------------------------------------------------------------------------

#ifndef ESGI_SHADER
#define ESGI_SHADER

// --- Includes --------------------------------------------------------------

#include <GL/glew.h>

// --- Classes ---------------------------------------------------------------

class EsgiShader
{
public:
	EsgiShader() : m_ProgramObject(0), m_VertexShader(0)
				, m_FragmentShader(0), m_GeometryShader(0)
	{
	}
	~EsgiShader()
	{
	}

	bool LoadVertexShader(const char *source);
	bool LoadFragmentShader(const char *source);
	bool LoadGeometryShader(const char *source);

	bool Create();
	void Destroy();

	GLuint Bind();
	void Unbind();

	inline GLuint GetProgram() const	{ return m_ProgramObject; }

	void SetVP(GLuint pVP);
	void SetM(GLuint pM);

	GLuint GetVP();
	GLuint GetM();
private:
	GLuint VP;
	GLuint M;

	// handle du program object
	GLuint m_ProgramObject;
	// handles des shaders
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_GeometryShader;
};

#endif // ESGI_SHADER