/**********************************************************************************
*
* Fragment Shader: Shockwave with Chromatic Aberration (WebGL 1.0)
*
* Creates an expanding circular shockwave from a center point using an SDF.
* This version is compatible with WebGL and uses the same uniform names as the
* desktop version.
*
**********************************************************************************/
#version 100

// Precision must be declared for floats in WebGL fragment shaders
precision mediump float;

// Input from vertex shader
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Uniforms from JS/C code - using the same names as the desktop shader
uniform sampler2D texture0;
uniform vec2 rippleCenter;
uniform float time;         // NOTE: This must be a NORMALIZED value (0.0 to 1.0)

// This helper function calculates the distortion strength at any point.
float getOffsetStrength(float t, vec2 dir) {
    const float maxRadius = 0.8;

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
    float rD = getOffsetStrength(time + tOffset, dir);
    float gD = getOffsetStrength(time, dir);
    float bD = getOffsetStrength(time - tOffset, dir);

    dir = normalize(dir);

    // Sample the texture for each channel at its distorted position
    // The texture sampling function is 'texture2D' in this version
    float r = texture2D(texture0, fragTexCoord + dir * rD).r;
    float g = texture2D(texture0, fragTexCoord + dir * gD).g;
    float b = texture2D(texture0, fragTexCoord + dir * bD).b;
    float a = texture2D(texture0, fragTexCoord + dir * gD).a;

    // Add a bright shading effect to the wave
    float shading = gD * 8.0;

    // Combine the final color and add the shading, assigning to the built-in gl_FragColor
    gl_FragColor = vec4(r, g, b, a) * fragColor;
    gl_FragColor.rgb += shading;
}


