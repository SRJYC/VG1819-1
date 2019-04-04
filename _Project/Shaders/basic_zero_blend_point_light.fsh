#version 150

in vec3 v_normal;
in vec3 v_pixelPos;
in vec2 v_uv1;

uniform sampler2D tex;
uniform float blendWeight0;

// lights

uniform vec3 light1Pos;
uniform vec4 light1Diffuse;
uniform vec4 lightAmbientColor;
uniform vec3 light1Attenuation;
uniform float light1Range;

uniform vec3 light2Pos;
uniform vec4 light2Diffuse;
uniform vec3 light2Attenuation;
uniform float light2Range;

// material
uniform vec4 matAmbient;
uniform vec4 matDiffuse;

out vec4 PixelColor;

void main()
{
	// ---- lighting -----
	vec4 light1Out;
	vec4 light2Out;
	
	vec4 ambient = lightAmbientColor * matAmbient;
	vec4 totalLightOut = ambient;
	
	// ===== Light 1 ======
	
	vec3 light1Dir = light1Pos - v_pixelPos;
	float light1Dist = length(light1Dir);
	
	if(light1Dist < light1Range)
	{
		//normalizing
		vec3 normal = normalize(v_normal);
		light1Dir /= light1Dist;
		
		vec4 diffuse = matDiffuse * light1Diffuse * max(0.0, dot(normal, light1Dir));
		light1Out = clamp(ambient + diffuse, 0, 1);
		
		//attenuation
		float atten = 1.0 / dot(light1Attenuation, vec3(1.0, light1Dist, light1Dist*light1Dist));
		
		light1Out *= atten;
		
		//set alpha to 1.0
		light1Out.w = 1.0;
	}
	
	// ===== Light 2 ======
	
	vec3 light2Dir = light2Pos - v_pixelPos;
	float light2Dist = length(light2Dir);
	
	if(light2Dist < light2Range)
	{
		//normalizing
		vec3 normal = normalize(v_normal);
		light2Dir /= light2Dist;
		
		vec4 diffuse = matDiffuse * light2Diffuse * max(0.0, dot(normal, light2Dir));
		light2Out = clamp(ambient + diffuse, 0, 1);
		
		//attenuation
		float atten = 1.0 / dot(light2Attenuation, vec3(1.0, light2Dist, light2Dist*light2Dist));
		
		light2Out *= atten;
		
		//set alpha to 1.0
		light2Out.w = 1.0;
	}
	
	// Blending
	vec4 firstTex = texture(tex, v_uv1);
	
	firstTex *= blendWeight0;
	
	totalLightOut += light1Out + light2Out;
	totalLightOut = clamp(totalLightOut, 0, 1);
	
	PixelColor = firstTex;
	PixelColor *= totalLightOut;
}
