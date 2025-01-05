#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;
uniform bool blinn;

struct Light {
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

void main() {
    // Phong Illumination 모델

    // ambient light 계산
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;
    
    vec3 result = ambient;

    vec3 lightDir = normalize(light.position - position);
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist * dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);

    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp(
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
        0.0, 1.0
    );

    if (intensity > 0.0)
    {
         // diffuse light 계산
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0f);
        vec3 diffuse = diff * texColor * light.diffuse;

        // specular light 계산
        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        vec3 viewDir = normalize(viewPos - position);
        float spec = 0.0;

        if (!blinn)
        {
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        }
        else
        {
            vec3 hafway = normalize(lightDir + viewDir);
            spec = pow(max(dot(pixelNorm, hafway), 0.0f), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        result += (diffuse + specular) * intensity;
    }

    result *= attenuation;
    fragColor = vec4(result, 1.0);
    //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}