/**********************************************************************************
*
* Fragment Shader: Shockwave with Chromatic Aberration (Desktop GLSL 3.30)
*
* Creates an expanding circular shockwave from a center point using an SDF.
* This version uses the uniform names from the previous ripple shader for
* compatibility with existing C code.
*
**********************************************************************************/
#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms from C code - using the original names
uniform sampler2D texture0;
uniform vec2 rippleCenter; // RENAMED: from 'centre'
uniform float time;         // RENAMED: from 't'. NOTE: This must be a NORMALIZED value (0.0 to 1.0)
uniform float maxIntensity; // NOTE: This is no longer used, as fade-out is controlled by 'time'

// Output to the screen
out vec4 finalColor;

// This helper function calculates the distortion strength at any point.
// It creates a thin, masked wave that fades in at the beginning and
// fades out at the end of its life, based on the normalized 'time'.
float getOffsetStrength(float t, vec2 dir) {
    const float maxRadius = 1.5;

    // Calculate the distance from the shockwave's edge (SDF of a circle)
    float d = length(dir) - t * maxRadius;

    // Mask the wave to give it a sharp, defined thickness
    d *= 1.0 - smoothstep(0.0, 0.05, abs(d));

    // Smoothly fade the effect in at the beginning
    d *= smoothstep(0.0, 0.05, t);

    // Smoothly fade the effect out as it nears the end of its life
    d *= 1.0 - smoothstep(0.8, 1.0, t);
    return d;
}

void main() {
    // Calculate the direction from the center to the current pixel
    vec2 dir = rippleCenter - fragTexCoord;

    // Create a slight time offset for the red and blue channels
    float tOffset = 0.05 * sin(time * 3.14);

    // Calculate the distortion strength for each color channel
    float rD = getOffsetStrength(time + tOffset, dir); // Red is slightly ahead
    float gD = getOffsetStrength(time, dir);           // Green is in the middle
    float bD = getOffsetStrength(time - tOffset, dir); // Blue is slightly behind

    dir = normalize(dir);

    // Sample the texture for each channel at its distorted position
    float r = texture(texture0, fragTexCoord + dir * rD).r;
    float g = texture(texture0, fragTexCoord + dir * gD).g;
    float b = texture(texture0, fragTexCoord + dir * bD).b;
    float a = texture(texture0, fragTexCoord + dir * gD).a;

    // Add a bright shading effect to the wave
    float shading = gD * 7.0;

    // Combine the final color and add the shading
    finalColor = vec4(r, g, b, a) * fragColor;
    finalColor.rgb += shading;
}


