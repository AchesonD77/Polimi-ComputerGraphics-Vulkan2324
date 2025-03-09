#version 450#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec4 fragTan;layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;layout(binding = 1) uniform sampler2D tex;
layout(binding = 3) uniform sampler2D spet;
layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;
} gubo;vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 T, vec3 B, vec3 Md, vec3 Ms, float alphaT, float alphaB) {/* This BRDF should perform the Ward anisotropic specular model with the Lambert diffuse model.		 Paramters:	V	   - Viewer direction	N	   - Normal vector direction	L	   - Light direction	T	   - Tangent vector direction	B	   - Bitangent vector direction	Md	   - Diffuse color	Ms	   - Specular color	alphaT - Roughness along the tangent direction	alphaB - Roughness along the bitangent direction*/	vec3 Diffuse = Md * clamp(dot(N, L),0.0,1.0);	vec3 Specular = Ms * vec3(pow(clamp(dot(V, -reflect(L, N)),0.0,1.0), 200.0f));	return (Diffuse + Specular);}
void main() {
	vec3 Norm = normalize(fragNorm);	vec3 Tan = normalize(fragTan.xyz - Norm * dot(fragTan.xyz, Norm));	vec3 Bitan = cross(Norm, Tan) * fragTan.w;	vec3 N = Norm;	vec3 albedo  = texture(tex,  fragUV).rgb;	vec3 specCol = texture(spet, fragUV).rgb;	vec3 L = gubo.lightDir;	vec3 lightColor = gubo.lightColor.rgb;	vec3 V = normalize(gubo.eyePos - fragPos);	vec3 DiffSpec = BRDF(V, N, L, Tan, Bitan, albedo, specCol, 0.1f, 0.4f);	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.15;	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.15;	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.15;	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.15;	const vec3 czp = vec3(0.8,0.2,0.4) * 0.15;	const vec3 czn = vec3(0.3,0.6,0.7) * 0.15;		vec3 Ambient =((N.x > 0 ? cxp : cxn) * (N.x * N.x) +				   (N.y > 0 ? cyp : cyn) * (N.y * N.y) +				   (N.z > 0 ? czp : czn) * (N.z * N.z)) * albedo;		outColor = vec4(DiffSpec * lightColor.rgb + Ambient, 1.0f);//	outColor = vec4(vec3(pex < 100.0f ? 0.0f : (pex < 200.0f ? 0.5f : 1.0f)),1.0f);//	outColor = vec4(vec3(pow(metallic, 1.0f/4.0f)), 1.0f);//	outColor = vec4(nMap.rgb, 1.0f);//	outColor = vec4(0.5f*N+vec3(0.5f), 1.0f);}