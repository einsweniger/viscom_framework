//llcXRl
/*--------------------------------------------------------------------------------------
License CC0 - http://creativecommons.org/publicdomain/zero/1.0/
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
----------------------------------------------------------------------------------------
^ This means do ANYTHING YOU WANT with this code. Because we are programmers, not lawyers.
-Otavio Good
*/

/*
This is a demo of the new font texture. It has a distance field and gradients in the texture.
Red channel:   Antialiased font if you just want something simple and easy to use.
Green channel: x gradient of distance field.
Blue channel:  y gradient of distance field.
Alpha channel: distance field.

The characters that are encoded are the characters from the first 8 bits of unicode (aka Latin-1 codepage).
That includes ASCII. In the blanks, there are symbols that seemed useful for shadertoy. Math symbols,
greek letters, play/pause controls, arrows, musical notes, and some others.

The distance field lets you use the font for ray marching. Since the texture didn't have enough precision
for a clean distance field, nice smooth gradients have been put in the green/blue channels to get smooth edges.

Sometimes you might see some artifacts in the font edges when you look from an angle. To fix those,
the ray marching would have to pause at the boundary between each letter since the distance field is
not continuous between letters. That would complicate this code a bit, so it was left out.
*/

vec3 GetReflection(vec3 rayDir)
{
	vec3 t = texture(tex_noise, -vec2(rayDir)).xyz;
	return vec3(t*t);
}


vec4 SampleFontTex(vec2 uv)
{
    const float GLYPHS_PER_UV = 16.;
    // Do some tricks with the UVs to spell out "TexFont" in the middle.
    vec2 fl = floor(uv + 0.5);
    if (fl.y == 0.0) {
        if (fl.x == -3.0) fl = vec2(4.0, 10.0);
        else if (fl.x == -2.0) fl = vec2(5.0, 9.0);
        else if (fl.x == -1.0) fl = vec2(8.0, 8.0);
        else if (fl.x == 0.0) fl = vec2(6.0, 11.0);
        else if (fl.x == 1.0) fl = vec2(15.0, 9.0);
        else if (fl.x == 2.0) fl = vec2(14.0, 9.0);
        else if (fl.x == 3.0) fl = vec2(4.0, 8.0);
    }
//    uv = fl + fract(uv+0.5)-0.5;

    // Sample the font texture. Make sure to not use mipmaps.
    // Add a small amount to the distance field to prevent a strange bug on some gpus. Slightly mysterious. :(
    return texture(tex_text, (uv+0.5)*(1.0/GLYPHS_PER_UV), -100.0) + vec4(0.0, 0.0, 0.0, 0.000000001);
}

// This is the distance function that defines all the scene's geometry.
// The input is a position in space.
// The output is the distance to the nearest surface and a material index.
subroutine(SceneMap)
vec2 sdfTextDemo(vec3 p)
{
	// Load the font texture's distance field.
    float letterDistField = (SampleFontTex(p.xy).w - 0.5+1.0/256.0);
    // intersect it with a box.
    float cropBox = sdfBox(p, vec3(0.5 + 5.0, 3.5, 0.25));
    vec2 letters = matmax(vec2(letterDistField, 42.0), vec2(cropBox, 0.3));
    return letters;
}

// Input is UV coordinate of pixel to render.
// Output is RGB color.
vec3 RayTrace(in vec2 fragCoord )
{
	vec3 camPos, camUp, camLookat;
	// ------------------- Set up the camera rays for ray marching --------------------
    // Map uv to [-1.0..1.0]
	vec2 uv = fragCoord.xy/iResolution.xy * 2.0 - 1.0;
    uv /= 2.0;  // zoom in
#define MANUAL_CAMERA
#ifdef MANUAL_CAMERA
    // Camera up vector.
	camUp=vec3(0,1,0);

	// Camera lookat.
	camLookat=vec3(0,0.0,0);

    // debugging camera
    float mx=-iMouse.x/iResolution.x*PI*2.0;
	float my=iMouse.y/iResolution.y*3.14*0.95 + PI/2.0;
	camPos = vec3(cos(my)*cos(mx),sin(my),cos(my)*sin(mx))*7.0;
#else
    // Camera up vector.
	camUp=vec3(0,1,0);

	// Camera lookat.
	camLookat=vec3(0,0,0);

    float camSpeed = 0.5;
	float camRad = 6.25;
    float camAngle = sin(iTime*camSpeed)*0.5;
	camPos = vec3(sin(camAngle)*camRad, 0.0, -cos(camAngle)*camRad);
#endif

	// Camera setup for ray tracing / marching
	vec3 camVec=normalize(camLookat - camPos);
	vec3 sideNorm=normalize(cross(camUp, camVec));
	vec3 upNorm=cross(camVec, sideNorm);
	vec3 worldFacing=(camPos + camVec);
	vec3 worldPix = worldFacing + uv.x * sideNorm * (iResolution.x/iResolution.y) + uv.y * upNorm;
	vec3 rayVec = normalize(worldPix - camPos);

	// ----------------------------- Ray march the scene ------------------------------
	vec2 distAndMat;  // Distance and material
	float t = 0.05;
	const float maxDepth = 16.0; // farthest distance rays will travel
	vec3 pos = camPos;
    const float smallVal = 1.0 / 16384.0;
    // ray marching time
    for (int i = 0; i <160; i++)	// This is the count of the max times the ray actually marches.
    {
        // Step along the ray.
        pos = (camPos + rayVec * t);
        // This is _the_ function that defines the "distance field".
        // It's really what makes the scene geometry. The idea is that the
        // distance field returns the distance to the closest object, and then
        // we know we are safe to "march" along the ray by that much distance
        // without hitting anything. We repeat this until we get really close
        // and then break because we have effectively hit the object.
        distAndMat = sdfTextDemo(pos);

        // move along the ray a safe amount
        t += distAndMat.x;
        // If we are very close to the object, let's call it a hit and exit this loop.
        if ((t > maxDepth) || (abs(distAndMat.x) < smallVal)) break;
    }

	// --------------------------------------------------------------------------------
	// Now that we have done our ray marching, let's put some color on this geometry.
	vec3 finalColor = vec3(0.0);

	// If a ray actually hit the object, let's light it.
    if (t <= maxDepth)
	{
        // calculate the normal from the distance field. The distance field is a volume, so if you
        // sample the current point and neighboring points, you can use the difference to get
        // the normal. This is called a gradient.
        vec3 smallVec = vec3(1.0/4096.0, 0, 0);
        vec3 normalU = vec3(sdfTextDemo(pos + smallVec.xyy).x - sdfTextDemo(pos - smallVec.xyy).x,
                           sdfTextDemo(pos + smallVec.yxy).x - sdfTextDemo(pos - smallVec.yxy).x,
                           sdfTextDemo(pos + smallVec.yyx).x - sdfTextDemo(pos - smallVec.yyx).x)*0.5;
        // If the material says we are on the edge of the font, override the normal with the
        // font texture's gradient. This will give us smoother surfaces on the sides of the font's letters.
        if (distAndMat.y == 0.0) {
            vec4 tx = SampleFontTex(pos.xy) - 0.5;
            // Put a small number in Z so it can't go to zero.
            normalU = -vec3(-tx.g, tx.b, 0.0001)*2.0*smallVec.x;
        }
        // It looks like ??texture filtering?? can sometimes take the normal to zero length.
        // Compensate so we don't divide by zero in the normalize function.
        normalU = normalU+0.000000001;
        vec3 normal = normalize(normalU);

        // ------ Calculate texture color  ------
        vec3 texColor = vec3(0.3,0.325,0.6)*0.5;
        if (distAndMat.y == 1.0) {
            // Make the "TexFont" word yellow
            if ((floor(pos.y+0.5) == 0.0) && (abs(floor(pos.x+0.5)) <= 3.5)) texColor = vec3(1.0,0.9,0.1);
        }

        // ------ Calculate lighting color ------
        // Sky color
        vec3 lightColor = (saturate(normal.y * 0.5 + 0.5)) * vec3(2.5);

        // Apply the light to the texture.
        finalColor = texColor * lightColor;

        // calculate the reflection vector for highlights
        vec3 ref = reflect(rayVec, normal);
        vec3 refColor = GetReflection(ref);
        refColor *= vec3(1.0, 0.5, 0.2)*2.5*dot(normal, -rayVec);
        finalColor += refColor;

        // debug visualize length of gradient of distance field to check distance field correctness
        //finalColor = vec3(0.5) * (length(normalU) / smallVec.x);
        //finalColor = normal * 0.5 + 0.5;
	}
    else
    {
        // Our ray trace hit nothing, so draw background.
        finalColor = GetReflection(rayVec);
    }

	// output the final color without gamma correction - will do gamma later.
	return vec3(clamp(finalColor, 0.0, 1.0));
}

void sdfTextMain( out vec4 fragColor, in vec2 fragCoord )
{
    vec3 finalColor = RayTrace(fragCoord);

    // Render 2d text.
    vec2 uv = fragCoord/iResolution.xy;
    // Render entire font texture for the first few seconds.
    float letters = texture(tex_text, uv, -1.0).x;
    finalColor = mix(finalColor, vec3(1.0, 1.0, 1.0)*letters, saturate(0.9-iTime*0.3));

    fragColor = vec4(sqrt(clamp(finalColor, 0.0, 1.0)),1.0);
}
