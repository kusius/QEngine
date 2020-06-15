#include "../Graphics/Model.h"
enum ModelType
{
    M_CHAIR
};

class ChairModel : public Model
{
public:
    ChairModel() : Model("Assets/chair/chair.fbx")
    {
        if (meshes.size() > 0)
        {
            vector<Texture> textures = loadMaterialTextures();
            meshes[0].textures.insert(meshes[0].textures.end(), textures.begin(), textures.end());
        }
    }

private:
    vector<Texture> loadMaterialTextures()
    {
        vector<Texture> textures;
        vector<string> textureNames{"T_Chair_03_BaseColor.png"};
        vector<string> textureTypes{"texture_diffuse"};
        string directory = "./Assets/chair/";

        for (unsigned int i = 0; i < textureNames.size(); i++)
        {
            Texture texture;
            texture.id = loadTextureFromFile(textureNames[i].c_str(), directory);
            texture.type = textureTypes[i];
            texture.path = textureNames[i].c_str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
        return textures;
    }
};