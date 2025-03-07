/***************************************************************************/
/*                                                                         */
/*  blinn.vs                                                               */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#version 440 compatibility

#include "Raptor.glsl"


LIGHT_PRODUCTS(0) R_LightProducts;


layout(location = 0) in vec4 i_Position;
layout(location = 2) in vec4 i_Normal;
layout(location = 8) in vec4 i_TexCoord;

out vec3 normal;
out vec4 o_texCoord;
out	vec4 lightDirs[MAX_LIGHTS];
out	vec3 halfVector[MAX_LIGHTS];

void main (void)
{
	normal = normalize(gl_NormalMatrix * vec3(i_Normal.xyz));
	vec3 ecPos = vec3(gl_ModelViewMatrix * i_Position);

	/*	This is true only for directional lights	*/
	/*halfVector = gl_LightSource[0].halfVector.xyz;*/

	for (int i=0 ; i<MAX_LIGHTS ; i++)
	{
		if (R_LightProducts.lights[i].enable)
		{
			vec3 ldir = vec3(R_LightProducts.lights[i].position) - ecPos;
			lightDirs[i].xyz = ldir;
			float dist = length(ldir);
			
			/*	In most situations, attenuation could be computed per vertex	*/
			vec4 attenuation = R_LightProducts.lights[i].attenuation;
			lightDirs[i].w = 1.0 / (	attenuation.z +
										attenuation.y * dist +
										attenuation.x * dist * dist);

			halfVector[i] = 0.5 * (normalize(-ecPos) + normalize(ldir));
		}
	}

	gl_Position = gl_ModelViewProjectionMatrix * i_Position;
	o_texCoord = i_TexCoord;
}


