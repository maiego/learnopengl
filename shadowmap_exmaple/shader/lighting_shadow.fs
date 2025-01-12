#version 330 core

in VS_OUT
{
    vec3 normal;
    vec2 texCoord;
    vec3 fragPos;
    vec4 fragPosLight;
} fs_in;

out vec4 fragColor;

uniform vec3 viewPos;
uniform bool blinn;
uniform sampler2D shadowMap;

struct Light {
    int directional;
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
 
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;


float shadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    float bias = max(0.02, 0.001 * (1.0 - dot(normal, lightDir)));

    float shadow = 0;
    int sampleN = 1;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int i = -sampleN; i <= sampleN; ++i)
    {
        for (int j = -sampleN; j <= sampleN; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= pow(2.0 * sampleN + 1.0, 2.0);
    return shadow;
}


void main() {
    // Blinn-Phong Illumination 모델

    // ambient light 계산
    vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
    vec3 ambient = texColor * light.ambient;
    
    vec3 result = ambient;

    vec3 lightDir;
    float intensity = 1.0;
    float attenuation = 1.0;

    if (light.directional == 1)
    {
        lightDir = normalize(-light.direction);
    }
    else
    {
        float dist = length(light.position - fs_in.fragPos);
        vec3 distPoly = vec3(1.0, dist, dist * dist);
        lightDir = normalize(light.position - fs_in.fragPos);
        attenuation = 1.0 / dot(distPoly, light.attenuation);
        
        float theta = dot(lightDir, normalize(-light.direction));
        intensity = clamp(
            (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
            0.0, 1.0);
    }


    if (intensity > 0.0)
    {
         // diffuse light 계산
        vec3 pixelNorm = normalize(fs_in.normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0f);
        vec3 diffuse = diff * texColor * light.diffuse;

        // specular light 계산
        vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
        vec3 viewDir = normalize(viewPos - fs_in.fragPos);
        float spec = 0.0;

        if (!blinn)
        {
            vec3 reflectDir = reflect(lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        }
        else
        {
            vec3 hafway = normalize(lightDir + viewDir);
            spec = pow(max(dot(pixelNorm, hafway), 0.0f), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        float shadow = shadowCalculation(fs_in.fragPosLight, pixelNorm, lightDir);
        result += (diffuse + specular) * intensity * (1.0 - shadow);
    }

    result *= attenuation;
    fragColor = vec4(result, 1.0);
    //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}