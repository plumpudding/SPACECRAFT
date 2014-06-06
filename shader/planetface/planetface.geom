#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform vec3 lightdir;

in vec3 pos[];
in vec4 gcolor[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];

out vec4 fcolor;

void main()
{
	// on n'affiche plus les blocs qui sont de l'autre côté de la planete
	vec4 PlanetCenter = view*model*vec4(0.0,0.0,0.0,1.0);
	vec4 BlocPosition = view*model*vec4(pos[0],1.0);
	if ( dot(BlocPosition,normalize(PlanetCenter)) > length(PlanetCenter) )
		return;

	
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];

	vec4 v[8];
	float c[5];

	v[0]=vec4(gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[1]=vec4(gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[2]=vec4(gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[3]=vec4(gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	v[4]=vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[5]=vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[6]=vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[7]=vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);

	const float ambient=0.0;
	c[0]=(max(dot(lightdir,normalize(vec3(v[0]))),0.0)+ambient)/255.0;
	c[1]=(max(dot(lightdir,normalize(vec3(v[1]-v[0]))),0.0)+ambient)/255.0;
	c[2]=(max(dot(lightdir,normalize(vec3(v[0]-v[1]))),0.0)+ambient)/255.0;
	c[3]=(max(dot(lightdir,normalize(vec3(v[0]-v[2]))),0.0)+ambient)/255.0;
	c[4]=(max(dot(lightdir,normalize(vec3(v[2]-v[0]))),0.0)+ambient)/255.0;
	
	mat4 projViewModel = proj*view*model;
	v[0]=projViewModel*v[0];
	v[1]=projViewModel*v[1];
	v[2]=projViewModel*v[2];
	v[3]=projViewModel*v[3];
	v[4]=projViewModel*v[4];
	v[5]=projViewModel*v[5];
	v[6]=projViewModel*v[6];
	v[7]=projViewModel*v[7];

	/*

	  6-------7
	 /|      /|
	4-------5 |
	| |     | |
	| 2-----|-3
	|/      |/
	0-------1

	*/

	fcolor.a = 1.0;

	fcolor.rgb = gcolor[0].rgb * vec3(84,124,60) * c[0];
	
	// TOP FACE
	gl_Position = logDepth(v[5]); EmitVertex();
	gl_Position = logDepth(v[4]); EmitVertex();
	gl_Position = logDepth(v[7]); EmitVertex();
	gl_Position = logDepth(v[6]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[1];

	// LEFT FACE
	gl_Position = logDepth(v[0]); EmitVertex();
	gl_Position = logDepth(v[2]); EmitVertex();
	gl_Position = logDepth(v[4]); EmitVertex();
	gl_Position = logDepth(v[6]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[2];

	// RIGHT FACE
	gl_Position = logDepth(v[3]); EmitVertex();
	gl_Position = logDepth(v[1]); EmitVertex();
	gl_Position = logDepth(v[7]); EmitVertex();
	gl_Position = logDepth(v[5]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[3];

	// FRONT FACE
	gl_Position = logDepth(v[1]); EmitVertex();
	gl_Position = logDepth(v[0]); EmitVertex();
	gl_Position = logDepth(v[5]); EmitVertex();
	gl_Position = logDepth(v[4]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[4];

	// BACK FACE
	gl_Position = logDepth(v[2]); EmitVertex();
	gl_Position = logDepth(v[3]); EmitVertex();
	gl_Position = logDepth(v[6]); EmitVertex();
	gl_Position = logDepth(v[7]); EmitVertex();
	EndPrimitive();
	
}
