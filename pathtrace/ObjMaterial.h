// Code from GEL (http://www.imm.dtu.dk/GEL/)
// Copyright (c) DTU Informatics 2011

#ifndef OBJMATERIAL_H
#define OBJMATERIAL_H

#include <string>

struct ObjMaterial
{
		/// Name of material
		std::string name;
		
		/// Diffuse reflection
		float diffuse[4];			
		
		/// Ambient reflection
		float ambient[4];
		
		/// Specular reflection
		float specular[4];		
		
		/// Specular exponent
		float shininess;			

    /// Index of refraction
    float ior;

    /// Transmission filter
    float transmission[3];

    /** Illumination model
          0 - Color
          1 - Color and ambient
          2 - Color and ambient and highlight
          3 - Reflection
          4 - Reflection and refraction
        Refer to the MTL format specification for more models. 
        Only numbers from 0 to 10 have a specific meaning. */
    int illum;
    
    bool has_texture;
		std::string tex_path, tex_name;
		int tex_id;

	ObjMaterial():	name("default"),
			 tex_path(""), tex_name(""), tex_id(-1), has_texture(false) 
				{
            ior = 1.5f;
						shininess = 0.0f;
						diffuse[0] = 0.8f;
						diffuse[1] = 0.8f;
						diffuse[2] = 0.8f;
						diffuse[3] = 1.0f;
						ambient[0] = 0.2f;
						ambient[1] = 0.2f;
						ambient[2] = 0.2f;
						ambient[3] = 1.0f;
						specular[0] = 0.0f;
						specular[1] = 0.0f;
						specular[2] = 0.0f;
						specular[3] = 1.0f;
            transmission[0] = 0.0f;
            transmission[1] = 0.0f;
            transmission[2] = 0.0f;
            illum = 2;
				}
};

#endif // OBJMATERIAL_H
