#version 330 core

in vec3 FragPos;
in vec3 TexCoord;

out vec4 FragColor;

uniform sampler3D volume;
uniform sampler1D transformation;
uniform vec3 lightPos;
uniform vec3 eyePos;

vec3 blinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor) { 
    vec3 ambient = vec3(0.3, 0.3, 0.3);
    vec3 diffuse = vec3(0.7, 0.7, 0.7);
    vec3 specular = vec3(0.3, 0.3, 0.3);
    float shininess = 32.0;

    float diff_coef = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec_coef = pow(max(dot(normal, halfDir), 0.0), shininess);

    return ambient + diff_coef * diffuse * lightColor + spec_coef * specular * lightColor;
}

void main() {
    // sample the normal from 3D texture
    vec4 GradWithIntensity = texture(volume, TexCoord);
    vec3 normal = GradWithIntensity.xyz;
    float intensity = GradWithIntensity.a;
    // sample the color from 1D texture
    vec4 color = texture(transformation, intensity);

    vec3 V = normalize(eyePos - FragPos);
    vec3 L = normalize(lightPos - FragPos);
    vec3 tempColor = color.rgb;
    tempColor = tempColor * blinnPhong(normal, L, V, vec3(0.8, 0.8, 0.8));

    vec4 finalColor = vec4(tempColor.rgb, color.a);
    FragColor = finalColor;
}