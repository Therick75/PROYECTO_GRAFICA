#version 330 core
out vec4 FragColor;

// Esta variable recibirá el color exacto desde nuestro código en C++
uniform vec4 objectColor;
uniform sampler2D texture1;
uniform bool useTexture;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 lighting = ambient + diffuse;

    vec4 baseColor;
    if (useTexture) {
        baseColor = texture(texture1, TexCoord);
    } else {
        baseColor = objectColor;
    }

    vec3 result = lighting * baseColor.rgb;
    FragColor = vec4(result, baseColor.a);
}