#version 330
layout(location = 0) out vec4 out_color;

uniform vec3 light_position;
uniform vec3 eye_position;

uniform sampler2D texture1;

uniform int fogSelector;
uniform int depthSelect;
uniform int vertexVsFragment;
uniform int depthFog;
uniform int depthFogChanges;
uniform int ffVertexFragment;

//can pass them as uniforms
const vec3 DiffuseLight = vec3(0.15, 0.05, 0.0);
const vec3 RimColor  = vec3(0.2, 0.2, 0.2);

in vec3 world_pos;
in vec3 world_normal;
in vec4 viewSpace;
in vec2 texcoord;
in float distVertex;
in float normalizedDepth;
in float fogFactorVF;

float fogFactorVertex=0;
const vec3 fogColor = vec3(0.5,0.5,0.5);


void main(){

	vec3 tex1 = texture(texture1, texcoord).rgb;

	//get light an view directions
	vec3 L = normalize( light_position - world_pos);
	vec3 V = normalize( eye_position - world_pos);

	//diffuse lighting
	vec3 diffuse = DiffuseLight * max(0, dot(L,world_normal));
	
	//rim lighting
	
	float rim = 1 - max(dot(V, world_normal), 0.0);
	rim = smoothstep(0.6, 1.0, rim);
	vec3 finalRim = RimColor * vec3(rim, rim, rim);


	//get all lights and texture
	vec3 finalColor = finalRim + diffuse + tex1;

	vec3 c = vec3(0,0,0);
	float dist =0 ;
	float fogFactor  = 0;

	if(vertexVsFragment == 1)
	{
		
		if(depthFog == 1)
		{
     		if(depthFogChanges == 0){
			 dist = abs(viewSpace.z);
			}
			else
			{
			  dist = (gl_FragCoord.z / gl_FragCoord.w);
			}
		   
			
		}
		else
		{
		     dist = length(viewSpace);
		}
	}
	else
	{
		 dist = distVertex;
	}

	if(fogSelector == 0)
	{
		fogFactor = (80 - dist)/(80 - 20);
		fogFactor = clamp( fogFactor, 0.0, 1.0 );

    	c = mix(fogColor, finalColor, fogFactor);
	}
	else if( fogSelector == 1)
	{
	   const float FogDensity = 0.05;
	   fogFactor = 1.0 /exp(dist * FogDensity);
	   fogFactor = clamp( fogFactor, 0.0, 1.0 );
	
	   c = mix(fogColor, finalColor, fogFactor);
	}
	else if( fogSelector == 2)
	{
	   const float FogDensity = 0.05;
	   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
	   fogFactor = clamp( fogFactor, 0.0, 1.0 ); 

	   //fogColor⋅(1−fogFactor)+finalColor⋅fogFactor
	   c = mix(fogColor, finalColor, fogFactor);
	}
	else if( fogSelector == 3)
	{
		
		   
		    //float be = (10.0 - viewSpace.y)*0.004;
			//float bi = (10.0 - viewSpace.y)*0.001;

			float be = 0.025 * smoothstep(0.0, 6.0, 10.0 - viewSpace.y);
			float bi = 0.035 * smoothstep(0.0, 80, 10.0 - viewSpace.y);
			float ext =  exp(-dist * be);
			float insc = exp(-dist * bi);
		
		    c = finalColor * ext + fogColor * (1 - insc);
		   
	}
	
	
		
	if(depthSelect == 1)
	{
		 fogFactor = 1 - fogFactor;
		 out_color = vec4( fogFactor, fogFactor, fogFactor,1.0 );
	}
	else
	{
		if(ffVertexFragment==1)
		{
		  c = mix(fogColor, finalColor, fogFactorVF);
		}

	
		out_color = vec4(c, 1);
	}
}