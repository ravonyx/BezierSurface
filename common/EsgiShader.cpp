// ---------------------------------------------------------------------------
//
// ESGI OpenGL (ES) 2.0 Framework
// Malek Bengougam, 2012							malek.bengougam@gmail.com
//
// ---------------------------------------------------------------------------

// --- Includes --------------------------------------------------------------

#include "EsgiShader.h"
#include <GL/glew.h>
#include <cstdio>
#include <cstdlib>

// --- Fonctions -------------------------------------------------------------

static char* FileToString(const char *sourceFile) 
{
     char* text = NULL;
     
     if (sourceFile != NULL) 
	 {
		 FILE *file = fopen(sourceFile, "r");
         
         if (file != NULL) 
		 {
             fseek(file, 0, SEEK_END);
             int count = ftell(file);
             rewind(file);
             
             if (count > 0) 
			 {
                 text = (char*)malloc(count + 1);
                 count = fread(text, sizeof(char), count, file);
                 text[count] = '\0';
             }

             fclose(file);
         }
     }

     return text;
}

///
// Cree un shader object, charge le code source du shader et le compile
//
static GLuint LoadShader(GLenum type, const char *sourceFile)
{
	// Preload le fichier de shader
	char *shaderSrc = FileToString(sourceFile);
	if (shaderSrc == NULL) {
		return false;
	}
	
	// Cree le shader object
	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		return 0;
	}

	// Load the shader source
	glShaderSource(shader, 1, (const char **)&shaderSrc, NULL);

	// Compile le shader
	glCompileShader(shader);

	// on n'a plus besoin du fichier texte
	free(shaderSrc);

	// verifie le status de la compilation
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) 
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = (char *)malloc(sizeof(char) * infoLen);

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);  

			free(infoLog);
		}

		// on supprime le shader object car il est inutilisable
		glDeleteShader(shader);
		
		return 0;
	}

	return shader;
}

bool EsgiShader::LoadVertexShader(const char *sourceFile)
{
	m_VertexShader = LoadShader(GL_VERTEX_SHADER, sourceFile);
	return (m_VertexShader > 0);
}


bool EsgiShader::LoadFragmentShader(const char *sourceFile)
{
	m_FragmentShader = LoadShader(GL_FRAGMENT_SHADER, sourceFile);
	return (m_FragmentShader != 0);
}

bool EsgiShader::LoadGeometryShader(const char *sourceFile)
{
	m_GeometryShader = LoadShader(GL_GEOMETRY_SHADER, sourceFile);
	return (m_GeometryShader != 0);
}

//
// Initialise les shader & program object
//
bool EsgiShader::Create()
{
	// Cree le program object
	m_ProgramObject = glCreateProgram();

	if (m_ProgramObject == 0) {
		return false;
	}

	if (m_VertexShader >= 0) {
		glAttachShader(m_ProgramObject, m_VertexShader);
	}
	if (m_FragmentShader >= 0) {
		glAttachShader(m_ProgramObject, m_FragmentShader);
	}
	if (m_GeometryShader >= 0) {
		glAttachShader(m_ProgramObject, m_GeometryShader);
	}

	// Liage des shaders dans le programme
	glLinkProgram(m_ProgramObject);

	GLint linked = 0;
	GLint infoLen = 0;

	// verification du statut du linkage
	glGetProgramiv(m_ProgramObject, GL_LINK_STATUS, &linked);

	if (!linked) 
	{
		glGetProgramiv(m_ProgramObject, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = (char *)malloc(sizeof(char) * infoLen);

			glGetProgramInfoLog(m_ProgramObject, infoLen, NULL, infoLog);
			printf("Erreur de lien du programme:\n%s\n", infoLog);                     

			free(infoLog);
		}

		glDeleteProgram(m_ProgramObject);

		return false;
	}

	
	glValidateProgram(m_ProgramObject);
	glGetProgramiv(m_ProgramObject, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 1)
	{
		char* infoLog = (char *)malloc(sizeof(char) * infoLen);

		glGetProgramInfoLog(m_ProgramObject, infoLen, NULL, infoLog);
		printf("Resultat de la validation du programme:\n%s\n", infoLog);                     

		free(infoLog);
	}
	return true;
}

//
// Libere la memoire occupee par le program et le shader object
//
void EsgiShader::Destroy()
{
	if (m_VertexShader) {
		glDetachShader(m_ProgramObject, m_VertexShader);
		glDeleteShader(m_VertexShader);
	}
	if (m_FragmentShader) {
		glDetachShader(m_ProgramObject, m_FragmentShader);
		glDeleteShader(m_FragmentShader);
	}
	if (m_GeometryShader) {
		glDetachShader(m_ProgramObject, m_GeometryShader);
		glDeleteShader(m_GeometryShader);
	}

	if (m_ProgramObject) {
		glDeleteProgram(m_ProgramObject);
	}
}

//
//
//
GLuint EsgiShader::Bind() {
     glUseProgram(m_ProgramObject);
	 return m_ProgramObject;
}

void EsgiShader::Unbind() {
     glUseProgram(0);
}

void EsgiShader::SetVP(GLuint pVP)
{
	VP = pVP;
}
void EsgiShader::SetM(GLuint pM)
{
	M = pM;
}

GLuint EsgiShader::GetVP()
{
	return VP;
}
GLuint EsgiShader::GetM()
{
	return M;
}