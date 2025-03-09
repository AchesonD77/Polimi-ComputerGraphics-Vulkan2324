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
vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {	vec3 Diffuse = Md * clamp(dot(N, L),0.0,1.0);	vec3 Specular = Ms * vec3(pow(clamp(dot(N, normalize(V + L)),0.0,1.0), gamma));		return (Diffuse + Specular);}
void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(texSampler, fragTexCoord).rgb;
	float pex = 200.0f;	float metallic = 1.0f;	vec3 L = gubo.lightDir;	vec3 lightColor = gubo.lightColor.rgb;	vec3 DiffSpec = BRDF(EyeDir, Norm, L, Albedo, vec3(metallic), pex);	// A special type of non-uniform ambient color, invented for this course	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.15;	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.15;	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.15;	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.15;	const vec3 czp = vec3(0.8,0.2,0.4) * 0.15;	const vec3 czn = vec3(0.3,0.6,0.7) * 0.15;		vec3 Ambient =((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +				   (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +				   (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) * Albedo;	
	outColor = vec4(DiffSpec * lightColor.rgb + Ambient, 1.0f);}