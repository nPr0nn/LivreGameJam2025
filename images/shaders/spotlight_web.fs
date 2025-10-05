/**********************************************************************************
*
* Fragment Shader: Spotlight Effect (WebGL 1.0 Version)
*
* This shader darkens the texture, except for a circular area around a
* specified center point, creating a "torchlight" or "spotlight" effect.
*
**********************************************************************************/

// WebGL 1.0 is based on an older version of GLSL
#version 100

// Precision must be declared for floats in WebGL fragment shaders
precision mediump float;

// Input attributes from the vertex shader
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Uniforms from the JavaScript/C code
uniform sampler2D texture0;    // The character's texture
uniform vec2 lightCenter;      // The center of the light, in texture coordinates (0.0 to 1.0)
uniform float lightRadius;     // The radius of the fully lit area
uniform float lightFalloff;    // The size of the soft edge, making the light fade out

void main() {
    // Get the original color of the pixel from the texture
    vec4 originalColor = texture2D(texture0, fragTexCoord);

    // Calculate the distance of this pixel from the light's center
    float distance = length(fragTexCoord - lightCenter);

    // Use smoothstep to create a soft, fading edge for the light.
    // It calculates an intensity value:
    // - 1.0 (fully lit) if distance is less than or equal to lightRadius.
    // - 0.0 (fully dark) if distance is greater than or equal to (lightRadius + lightFalloff).
    // - A smooth transition between 1.0 and 0.0 inside the falloff area.
    float lightIntensity = 1.0 - smoothstep(lightRadius, lightRadius + lightFalloff, distance);

    // Multiply the original color's RGB values by the calculated light intensity.
    // The alpha (transparency) of the original texture is preserved.
    gl_FragColor = vec4(originalColor.rgb * lightIntensity, originalColor.a) * fragColor;
}

