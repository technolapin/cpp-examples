
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
////////////////////////////////////////////////////////////
// from assimp opengl sample code

/* the global Assimp scene object */

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void get_bounding_box_for_node (const C_STRUCT aiScene * scene,
                                const C_STRUCT aiNode* nd,
	C_STRUCT aiVector3D* min,
	C_STRUCT aiVector3D* max,
	C_STRUCT aiMatrix4x4* trafo
){
	C_STRUCT aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const C_STRUCT aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			C_STRUCT aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
            get_bounding_box_for_node(scene, nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}
void get_bounding_box(const C_STRUCT aiScene * scene,
                      C_STRUCT aiVector3D* min,
                      C_STRUCT aiVector3D* max)
{
	C_STRUCT aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene, scene->mRootNode,min,max,&trafo);
}

void
loadasset (const C_STRUCT aiScene ** scene_ptr,
           const std::string path)
{
    /* we are taking one of the postprocessing presets to avoid
       spelling out 20+ single postprocessing flags here. */
    *scene_ptr = aiImportFile(path.c_str(),
                             aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene_ptr) {
        C_STRUCT aiVector3D scene_min, scene_max, scene_center;
        get_bounding_box(*scene_ptr, &scene_min,&scene_max);
        scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
        scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
        scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
    }
    else
    {
        throw std::string("could not open file '") + path + "' !";
    }
    
}
///////////////////////////////////////////////////////////////////
int
main(const int argc,
     const char ** argv)
{
    
    const C_STRUCT aiScene * scene = NULL;

    if (argc != 2)
    {
        std::cerr << "Expected one argument (a 3d model)" <<std::endl;
    }
    
    try
    {
        loadasset(&scene, argv[1]);
    }
    catch(const std::string e)
    {
        std::cerr << e  << std::endl;
        return -1;
    }

    std::cout << scene << std::endl;


    /*
      There can be a lot of things in a scene.
      Here we assume that there is only one list of meshes
      and of materials because we just want to import models.
      
      You must look at assimp's source to use it (do not worry it is not that bad)

      https://github.com/assimp/assimp/blob/master/include/assimp/scene.h
      https://github.com/assimp/assimp/blob/master/include/assimp/mesh.h
      https://github.com/assimp/assimp/blob/master/include/assimp/material.h

      The C_STRUCTS are just for doxygen documentation, those can be removed.
     */

    // looking at the meshes
    std::cout << "There are " << scene->mNumMeshes << " meshes" << std::endl;
    for (uint32_t i_mesh = 0;
         i_mesh < scene->mNumMeshes;
         ++i_mesh)
    {
        const C_STRUCT aiMesh * mesh = scene->mMeshes[i_mesh];

        // looking at the faces
        for (uint32_t i_face = 0;
             i_face < mesh->mNumFaces;
             ++i_face)
        {
            std::cout << "i_face " << i_face << std::endl;
            // there are always faces
            const C_STRUCT aiFace & face = mesh->mFaces[i_face];
            std::cout << "number indice on this face: " << face.mNumIndices << std::endl;
            for (uint32_t i_faceindice = 0;
                 i_faceindice < face.mNumIndices;
                 ++i_faceindice)
            {
                std::cout << face.mIndices[i_faceindice] << " ";
            }
            std::cout << std::endl;
        }
        
        // looking at the vertices
        for (uint32_t i_vert = 0;
             i_vert < mesh->mNumVertices;
             ++i_vert)
        {
            std::cout << "i_vert " << i_vert << std::endl;
            // there is always a position
            // there can optionaly be normals, colors, tangents and some other things
            // When something is not given by the 3D model, the pointer is set to nullptr
            // check in the mesh.h file of assimp for all the possible fields
            const C_STRUCT aiVector3D & pos = mesh->mVertices[i_vert];
            const C_STRUCT aiVector3D & nor = mesh->mNormals[i_vert];
            std::cout << "pos: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
            std::cout << "nor: " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
        }

        std::cout << "The mesh uses the material n°" << mesh->mMaterialIndex << std::endl;
    }
    std::cout << "####################################" << std::endl;
    // looking at the materials
    for (uint32_t i_mat = 0;
         i_mat < scene->mNumMaterials;
         ++i_mat)
    {
        const C_STRUCT aiMaterial * mat = scene->mMaterials[i_mat];
        std::cout << "i_mat: " << i_mat << std::endl;
        std::cout << "Material name: " << mat->GetName().C_Str() << std::endl;

        // example: fetch the diffuse color from the material (assuming there is a diffuse color)
        // to find the AI_MATKEY_* parameter corresponding to the material you want, searck in the documentation or the source of material.h
        aiColor3D diffuse;
        if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) != AI_SUCCESS)
        {
            throw "Material field COLOR_DIFFUSE not found!";
        }
        else
        {
            std::cout << "diffuse: " << diffuse.r << " " << diffuse.g << " " <<  diffuse.b << std::endl;
        }
    }

    
    return 0;
}

