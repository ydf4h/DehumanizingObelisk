#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <OBJ_parser.hpp>
#include <model.hpp>

unsigned int count_nr_substr(const std::string member, const std::string habitat){
    unsigned int count = 0;

    size_t substr_pos = habitat.find(member);

    while(habitat.find(member, substr_pos) != std::string::npos){
        count++;
        substr_pos = habitat.find(member, substr_pos + member.length());
    }

    return count;
}//returns how many times a substring occurs in the given string

void loadOBJ(const std::string filepath, Model* modelin){
    std::ifstream fileIn;
    std::stringstream fileStringStream;
    std::string filetosearch;

    std::cout << typeid(*modelin).name() << '\n';

    unsigned int o_lines;

    fileIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        fileIn.open(filepath);

        fileStringStream << fileIn.rdbuf();

        filetosearch = fileStringStream.str();

        o_lines = count_nr_substr("\no ", filetosearch);

        fileIn.close();
    }catch(std::ifstream::failure e){
        std::cout << "Failed to read from requested file.\n";
    }

    size_t o_line_pos = filetosearch.find("\no ");

    for(unsigned int o = 0; o < o_lines; o++){
        Mesh meshOut;

        unsigned int mesh_end_pos = filetosearch.find("\no ", o_line_pos + 3);
        std::string meshString = filetosearch.substr(o_line_pos, mesh_end_pos - o_line_pos);

        std::vector<std::string> existing_v_combs; //Used to know which vertex combinations already exist for face parsing

        size_t v_line_pos = filetosearch.find("\nv ");
        size_t vn_line_pos = filetosearch.find("\nvn ");
        size_t vt_line_pos = filetosearch.find("\nvt ");

        size_t f_line_pos = filetosearch.find("\nf ");

        unsigned int v_lines = count_nr_substr("\nv ", filetosearch);
        unsigned int vn_lines = count_nr_substr("\nvn ", filetosearch);
        unsigned int vt_lines = count_nr_substr("\nvt ", filetosearch);
        unsigned int f_lines = count_nr_substr("\nf ", filetosearch);

        std::vector<VertexComponent> vertexPositions;
        std::vector<VertexComponent> vertexNormals;
        std::vector<VertexComponent> vertexUVs; //Temporarily store the vertex values for each type

        std::vector<std::string> existing_f_combs;
        unsigned int indices_count_up = 0; //Used for the creation of indices

        std::vector<float> vertexData;
        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < v_lines; i++){
            unsigned int line_end_pos = filetosearch.find('\n', v_line_pos + 3);

            std::string current_line = filetosearch.substr(v_line_pos, line_end_pos - v_line_pos);

            VertexComponent vpos;

            size_t space1 = current_line.find(" ");
            size_t space2 = current_line.find(" ", space1 + 1);
            size_t space3 = current_line.find(" ", space2 + 1);

            std::string vPosX_str = current_line.substr(space1, space2 - space1);
            std::string vPosY_str = current_line.substr(space2, space3 - space2);
            std::string vPosZ_str = current_line.substr(space3, line_end_pos - space3);

            float vPosX = std::stof(vPosX_str);
            float vPosY = std::stof(vPosY_str);
            float vPosZ = std::stof(vPosZ_str);

            vpos.pos.push_back(vPosX);
            vpos.pos.push_back(vPosY);
            vpos.pos.push_back(vPosZ);

            vertexPositions.push_back(vpos);

            v_line_pos = filetosearch.find("\nv ", line_end_pos);
        }

        for(unsigned int i = 0; i < vn_lines; i++){
            unsigned int line_end_pos = filetosearch.find('\n', vn_line_pos + 4);

            std::string current_line = filetosearch.substr(vn_line_pos, line_end_pos - vn_line_pos);

            VertexComponent vn;

            size_t space1 = current_line.find(" ");
            size_t space2 = current_line.find(" ", space1 + 1);
            size_t space3 = current_line.find(" ", space2 + 1);

            std::string vNormX_str = current_line.substr(space1, space2 - space1);
            std::string vNormY_str = current_line.substr(space2, space3 - space2);
            std::string vNormZ_str = current_line.substr(space3, line_end_pos - space3);

            float vNormX = std::stof(vNormX_str);
            float vNormY = std::stof(vNormY_str);
            float vNormZ = std::stof(vNormZ_str);

            vn.pos.push_back(vNormX);
            vn.pos.push_back(vNormY);
            vn.pos.push_back(vNormZ);

            vertexNormals.push_back(vn);

            vn_line_pos = filetosearch.find("\nvn ", line_end_pos);
        }

        for(unsigned int i = 0; i < vt_lines; i++){
            unsigned int line_end_pos = filetosearch.find('\n', vt_line_pos + 4);

            std::string current_line = filetosearch.substr(vt_line_pos, line_end_pos - vt_line_pos);

            VertexComponent vt;

            size_t space1 = current_line.find(" ");
            size_t space2 = current_line.find(" ", space1 + 1);

            std::string vUV_X_str = current_line.substr(space1, space2 - space1);
            std::string vUV_Y_str = current_line.substr(space2, line_end_pos - space2);

            float vUV_X = std::stof(vUV_X_str);
            float vUV_Y = std::stof(vUV_Y_str);

            vt.pos.push_back(vUV_X);
            vt.pos.push_back(vUV_Y);

            vertexUVs.push_back(vt);

            vt_line_pos = filetosearch.find("\nvt ", line_end_pos);
        }

        //    Alright, parsing faces is complicated so get ready. Get the string of each vertex for each face which is "f vpos/vUV/vnorm",
        // each face should have 3 of the v/v/v things. When loading each vertex like that loop through a table containing all the combinations
        // and check if one already exists for the current one, if so, then a new value will not be created for that.
        for(unsigned int i = 0; i < f_lines; i++){
            unsigned int line_end_pos = filetosearch.find('\n', f_line_pos + 3);

            std::vector<std::string> vertexIndices_str;

            std::string current_line = filetosearch.substr(f_line_pos, line_end_pos - f_line_pos);

            size_t space1 = current_line.find(" ");
            size_t space2 = current_line.find(" ", space1 + 1);
            size_t space3 = current_line.find(" ", space2 + 1);

            std::string vertex1str = current_line.substr(space1, space2 - space1);
            vertexIndices_str.push_back(vertex1str);
            std::string vertex2str = current_line.substr(space2, space3 - space2);
            vertexIndices_str.push_back(vertex2str);
            std::string vertex3str = current_line.substr(space3, line_end_pos);
            vertexIndices_str.push_back(vertex3str);

            for(unsigned int j = 0; j < 3; j++){
                size_t slash1 = vertexIndices_str[j].find('/');
                size_t slash2 = vertexIndices_str[j].find('/', slash1 + 1);

                auto search = std::find(existing_f_combs.begin(), existing_f_combs.end(), vertexIndices_str[j]);
                if(search != existing_f_combs.end()){
                    size_t search_index = std::distance(existing_f_combs.begin(), search);
                    indices.push_back(search_index);
                }else{
                    existing_f_combs.push_back(vertexIndices_str[j]);

                    indices.push_back(indices_count_up);
                    indices_count_up++;

                    std::string vPosInd_str = vertexIndices_str[j].substr(0, slash1);
                    std::string vUV_Ind_str = vertexIndices_str[j].substr(slash1 + 1, slash2 - slash1 - 1);
                    std::string vNormInd_str = vertexIndices_str[j].substr(slash2 + 1);

                    unsigned int vPosInd = std::stoul(vPosInd_str) - 1;
                    unsigned int vUV_Ind = std::stoul(vUV_Ind_str) - 1;
                    unsigned int vNormInd = std::stoul(vNormInd_str) - 1; //std::stoul converts string to unsigned int

                    for(unsigned int l = 0; l < 3; l++){
                        vertexData.push_back(vertexPositions[vPosInd].pos[l]);
                    }

                    for(unsigned int l = 0; l < 3; l++){
                        vertexData.push_back(vertexNormals[vNormInd].pos[l]);
                    }

                    for(unsigned int l = 0; l < 2; l++){
                        vertexData.push_back(vertexUVs[vUV_Ind].pos[l]);
                    }
                }
            }

            f_line_pos = filetosearch.find("\nf ", line_end_pos);
        }

        meshOut.vertexData = vertexData;
        meshOut.indices = indices;

        meshOut.parent = modelin;

        modelin->meshes.push_back(meshOut);

        o_line_pos = filetosearch.find("\no ", mesh_end_pos);
    }
}