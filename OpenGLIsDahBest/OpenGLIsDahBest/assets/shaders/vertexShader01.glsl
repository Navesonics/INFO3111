// Vertex shader
#version 420

uniform mat4x4 matModel;			// M : where's my model at?
uniform mat4x4 matView;				// V : where's the camera at?
uniform mat4x4 matProjection;		// P : what's the screen doing?
//uniform mat4 MVP;

// Coming in from the vertex layout
in vec3 vCol;		
in vec3 vPos;
in vec3 vNormal;

// Going out to the Fragment shader
out vec3 color;
out vec3 fNormal;		// 'f' for fun or fromage or fragment
out vec4 fVertexPosWorld;		// Where this vertex is in 'world space'

void main()
{
	vec3 position = vPos;
	
//	vec4 = mat4x4 * vec4;
// From line 439 of the C++ main file
// mvp = p * v * m;
	
	mat4 matMVP = matProjection * matView * matModel;
	
	// The location of the vertex in 'world' space
	fVertexPosWorld = matModel * vec4(position, 1.0);
	
	// Location of vertex in 'screen space'
    gl_Position = matMVP * vec4(position, 1.0);
	
    color = vCol;
	fNormal = vNormal;
	
}// off to the fragment shader