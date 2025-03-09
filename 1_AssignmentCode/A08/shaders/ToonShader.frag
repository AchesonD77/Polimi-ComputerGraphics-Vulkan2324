#version 450#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;} gubo;
vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {/* This BRDF should perform toon shading with the following charcteristics:Lets call cos(alpha) the cosine of the angle between the normal vector and the light vector,and cos(beta) the cosine of the angle between the specular reflection vector and the viewer direction.The toon will return the sum of a pecentage of the diffuse and a percentage of the specular colors,according to the following proportions:Diffuse -   0% if cos(alpha) is zero or negative        -   gradient between 0% and 15% if 0 < cos(alpha) <= 0.1		-  15% if 0.1 < cos(alpha) <= 0.7		-   gradient between 15% and 100% if 0.7 < cos(alpha) <= 0.8		- 100% if cos(alpha) > 0.8Specular -   0% if cos(beta) <= 0.9		 -   gradient between 0% and 100% if 0.9 < cos(alpha) <= 0.95		 - 100% if cos(beta > 0.95)		 Paramters:	V	- Viewer direction	N	- Normal vector direction	L	- Light direction	Md	- Diffuse color	Ms	- Specular color*/	vec3 Diffuse = Md * clamp(dot(N, L),0.0,1.0);	vec3 Specular = Ms * vec3(pow(clamp(dot(V, -reflect(L, N)),0.0,1.0), 200.0f)); 		return (Diffuse + Specular);}
void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(texSampler, fragTexCoord).rgb;
	vec3 L = gubo.lightDir;	vec3 lightColor = gubo.lightColor.rgb;	vec3 DiffSpec = BRDF(EyeDir, Norm, L, Albedo, vec3(1.0f));	// A special type of non-uniform ambient color, invented for this course	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.15;	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.15;	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.15;	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.15;	const vec3 czp = vec3(0.8,0.2,0.4) * 0.15;	const vec3 czn = vec3(0.3,0.6,0.7) * 0.15;		vec3 Ambient =((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +				   (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +				   (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) * Albedo;	
	outColor = vec4(DiffSpec * lightColor.rgb + Ambient, 1.0f);	//outColor = vec4(0.5f*Norm+vec3(0.5f), 1.0f);}