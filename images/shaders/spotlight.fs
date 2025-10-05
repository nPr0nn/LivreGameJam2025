/**********************************************************************************
*
* Fragment Shader: Spotlight Effect (Desktop GLSL 3.30 Version)
*
* This shader darkens the texture, except for a circular area around a
* specified center point, creating a "torchlight" or "spotlight" effect.
*
**********************************************************************************/

// Use a modern GLSL version common for desktop applications
#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms from the C code
uniform sampler2D texture0;    // The character's texture
uniform vec2 lightCenter;      // The center of the light, in texture coordinates (0.0 to 1.0)
uniform float lightRadius;     // The radius of the fully lit area
uniform float lightFalloff;    // The size of the soft edge, making the light fade out

// Output color for the fragment
out vec4 finalColor;

void main() {
    // Get the original color of the pixel from the texture
    // The texture sampling function is just 'texture' in this version
    vec4 originalColor = texture(texture0, fragTexCoord);

    // Calculate the distance of this pixel from the light's center
    float distance = length(fragTexCoord - lightCenter);

    // Use smoothstep to create a soft, fading edge for the light.
    // The logic here is identical to the web version.
    float lightIntensity = 1.0 - smoothstep(lightRadius, lightRadius + lightFalloff, distance);

    // Multiply the original color's RGB values by the calculated light intensity
    // and assign the result to our custom output variable.
    finalColor = vec4(originalColor.rgb * lightIntensity, originalColor.a) * fragColor;
}

