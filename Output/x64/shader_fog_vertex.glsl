#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_normal;		
layout(location = 2) in vec2 in_texcoord;

uniform mat4 model_matrix, view_matrix,view_matrix2, projection_matrix;

uniform int vertexVsFragment;
uniform int fogSelector;
uniform int cameraSelect;
uniform int depthFog;
uniform int ffVertexFragment;

out vec3 world_pos;
out vec3 world_normal;
out vec2 texcoord;
out vec4 viewSpace;
out float distVertex;
out float fogFactorVertex;
out float normalizedDepth;
out float fogFactorVF;
void main(){

	 world_pos = (model_matrix * vec4(in_position,1)).xyz;
	 world_normal = normalize(mat3(model_matrix) * in_normal);
	 texcoord = in_texcoord;

	 viewSpace = view_matrix * model_matrix * vec4(in_position,1);
	 
	if(cameraSelect == 1)
		gl_Position = projection_matrix*view_matrix2*model_matrix*vec4(in_position,1); 
	else
		gl_Position = projection_matrix * viewSpace; 

    //just  trying something
	normalizedDepth = (gl_Position.z /gl_Position.w);
	
	if(vertexVsFragment == 0)
	{
	    //compute depth
		if(depthFog == 1)
		{
			//get another type of distance
			//distVertex = (gl_Position.z /gl_Position.w)*gl_DepthRange.far*20;
			distVertex = abs(viewSpace.z);
		}
		else
		{
		    distVertex = length(viewSpace);
		}


	}

	//compute factor in vertex shader
	if(ffVertexFragment == 1)
	{
		if(fogSelector == 0)
		{
			fogFactorVF = (80 - distVertex)/(80 - 20);
			fogFactorVF = clamp( fogFactorVF, 0.0, 1.0 );
			    		
		}
		else if( fogSelector == 1)
		{
		   const float FogDensity = 0.05;
		   fogFactorVF = 1.0 /exp(distVertex * FogDensity);
		   fogFactorVF = clamp( fogFactorVF, 0.0, 1.0 );
			   
		}
		else if( fogSelector == 2)
		{
		   const float FogDensity = 0.05;
		   fogFactorVF = 1.0 /exp( (distVertex * FogDensity)* (distVertex * FogDensity));
		   fogFactorVF = clamp( fogFactorVF, 0.0, 1.0 ); 
		   		  
		}
	}
	
	
	
}
