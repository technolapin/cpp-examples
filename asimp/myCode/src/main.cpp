
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

aiScene
loadasset (const std::string path)
{
    aiScene scene;
    const aiScene* scene_ptr = &scene;
    /* we are taking one of the postprocessing presets to avoid
       spelling out 20+ single postprocessing flags here. */
    scene_ptr = aiImportFile(path.c_str(),
                             aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene_ptr) {
        C_STRUCT aiVector3D scene_min, scene_max, scene_center;
        get_bounding_box(scene_ptr, &scene_min,&scene_max);
        scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
        scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
        scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
    }
    else
    {
        throw std::string("could not open file '") + path + "' !";
    }
    
    return scene;
}
///////////////////////////////////////////////////////////////////

int
main(const int argc,
     const char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "Expected one argument (a 3d model)" <<std::endl;
    }
    try
    {
        const aiScene scene = loadasset(argv[1]);
    }
    catch(const std::string e)
    {
        std::cerr << e  << std::endl;
    }

    return 0;
}

