#version 400

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex
in vec3 ex_Jet_LightPos;  //wing light direction arriving from the vertex

in vec3 ex_PositionEye;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 jet_light_diffuse;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;

uniform sampler2D DiffuseMap;

void main(void)
{
	//Calculate lighting
	vec3 n, L, jL;
	vec4 color;
	float NdotL, NdotJL;
	float dist;
	float att;

	dist = length(ex_Jet_LightPos - ex_PositionEye);
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);
	jL = normalize(ex_Jet_LightPos - ex_PositionEye);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);
	NdotJL = max(dot(n, jL),0.0);

	color = light_ambient * material_ambient;
	
	if(NdotL > 0.0)
	{
		color += (light_ambient * material_diffuse * NdotL);
	}
	
	if(NdotJL > 0.0)
	{
		att = min(1.0 / (constantAttenuation + (linearAttenuation * dist) + (quadraticAttenuation * (dist * dist))),1);
		color += att*(jet_light_diffuse * material_diffuse * NdotJL);
	}

	color += material_specular * light_specular * pow(RdotV, material_shininess);

    out_Color = color * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}