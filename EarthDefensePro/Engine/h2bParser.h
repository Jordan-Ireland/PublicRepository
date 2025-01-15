#ifndef _H2BPARSER_H_
#define _H2BPARSER_H_
#include <fstream>
#include <vector>
#include <set>

namespace H2B {

#pragma pack(push,1)
	struct VECTOR {
		float x, y, z;
	};
	struct VERTEX {
		VECTOR pos, uvw, nrm;
	};
	struct alignas(void*) ATTRIBUTES {
		VECTOR Kd; float d;
		VECTOR Ks; float Ns;
		VECTOR Ka; float sharpness;
		VECTOR Tf; float Ni;
		VECTOR Ke; unsigned illum;
	};
	struct BATCH {
		unsigned indexCount, indexOffset;
	};
#pragma pack(pop)
	struct MATERIAL {
		ATTRIBUTES attrib;
		std::string name;
		std::string map_Kd;
		std::string map_Ks;
		std::string map_Ka;
		const char* map_Ke;
		std::string map_Ns;
		const char* map_d;
		const char* disp;
		const char* decal;
		const char* bump;
		const void* padding[2];
	};
	struct MESH {
		const char* name;
		BATCH drawInfo;
		unsigned materialIndex;
	};
	class Parser
	{
		std::set<std::string> file_strings;
	public:
		char version[4];
		unsigned vertexCount;
		unsigned indexCount;
		unsigned materialCount;
		unsigned meshCount;
		std::vector<VERTEX> vertices;
		std::vector<unsigned> indices;
		std::vector<MATERIAL> materials;
		std::vector<BATCH> batches;
		std::vector<MESH> meshes;
		bool Parse(const char* h2bPath)
		{
			Clear();
			std::ifstream file;
			char buffer[260] = { 0, };
			file.open(h2bPath, std::ios_base::in |
				std::ios_base::binary);
			if (file.is_open() == false)
				return false;
			file.read(version, 4);
			if (version[1] < '1' || version[2] < '9' || version[3] < 'd')
				return false;
			file.read(reinterpret_cast<char*>(&vertexCount), 4);
			file.read(reinterpret_cast<char*>(&indexCount), 4);
			file.read(reinterpret_cast<char*>(&materialCount), 4);
			file.read(reinterpret_cast<char*>(&meshCount), 4);
			vertices.resize(vertexCount);
			file.read(reinterpret_cast<char*>(vertices.data()), 36 * vertexCount);
			indices.resize(indexCount);
			file.read(reinterpret_cast<char*>(indices.data()), 4 * indexCount);
			materials.resize(materialCount);
			for (int i = 0; i < materialCount; ++i) {
				file.read(reinterpret_cast<char*>(&materials[i].attrib), 80);

				//get material name
				buffer[0] = '\0';
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					//file_strings.insert(buffer);
					materials[i].name = buffer;// *file_strings.rbegin();
				}

				//get diffuse texture
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					std::string fileName = convertToString(buffer, 260);
					fileName = fileName.substr(fileName.rfind("/") + 1);
					fileName = fileName.substr(0, fileName.find("."));
					fileName += ".dds";
					//file_strings.insert(fileName);
					materials[i].map_Kd = fileName;// *file_strings.rbegin();
				}

				//get specular texture
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					std::string fileName = convertToString(buffer, 260);
					fileName = fileName.substr(fileName.rfind("/") + 1);
					fileName = fileName.substr(0, fileName.find("."));
					fileName += ".dds";
					//file_strings.insert(fileName);
					materials[i].map_Ks = fileName;// *file_strings.rbegin();
				}

				//get ambient texture
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					std::string fileName = convertToString(buffer, 260);
					fileName = fileName.substr(fileName.rfind("/") + 1);
					fileName = fileName.substr(0, fileName.find("."));
					fileName += ".dds";
					//file_strings.insert(fileName);
					materials[i].map_Ka = fileName;// *file_strings.rbegin();
				}

				//skip map_Ke
				file.getline(buffer, 260, '\0');

				//get shininess(metal) texture
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					std::string fileName = convertToString(buffer, 260);
					fileName = fileName.substr(fileName.rfind("/") + 1);
					fileName = fileName.substr(0, fileName.find("."));
					fileName += ".dds";
					//file_strings.insert(fileName);
					materials[i].map_Ns = fileName;// *file_strings.rbegin();
				}

				//ignore other texture fields
				for (int j = 0; j < 5; ++j) {
					file.getline(buffer, 260, '\0');
				}
			}
			batches.resize(materialCount);
			file.read(reinterpret_cast<char*>(batches.data()), 8 * materialCount);
			meshes.resize(meshCount);
			for (int i = 0; i < meshCount; ++i) {
				buffer[0] = '\0';
				meshes[i].name = nullptr;
				file.getline(buffer, 260, '\0');
				if (buffer[0] != '\0') {
					auto last = file_strings.insert(buffer);
					meshes[i].name = last.first->c_str();
				}
				file.read(reinterpret_cast<char*>(&meshes[i].drawInfo), 8);
				file.read(reinterpret_cast<char*>(&meshes[i].materialIndex), 4);
			}
			return true;
		}
		void Clear()
		{
			*reinterpret_cast<unsigned*>(version) = 0;
			file_strings.clear();
			vertices.clear();
			indices.clear();
			materials.clear();
			batches.clear();
			meshes.clear();
		}

		std::string convertToString(char* a, int size) {
			int i;
			std::string s = "";
			for (i = 0; i < size; i++) {
				s = s + a[i];
			}
			return s;
		}
	};
}
#endif