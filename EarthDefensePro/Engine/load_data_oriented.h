// This is a sample of how to load a level in a data oriented fashion.
// Feel free to use this code as a base and tweak it for your needs.
// *NEW* The new version of this loader saves blender names.

// This reads .h2b files which are optimized binary .obj+.mtl files

#include "h2bParser.h"
#include "TextureArray.h"
#include "Defines.h"

class Level_Data {

	// transfered from parser
	std::set<std::string> level_strings;
public:
	struct LEVEL_MODEL // one model in the level
	{
		const char* filename; // .h2b file data was pulled from
		unsigned vertexCount, indexCount, materialCount, meshCount;
		unsigned vertexStart, indexStart, materialStart, meshStart, batchStart;
		unsigned colliderIndex; // *NEW* location of OBB in levelColliders
		uint32_t useTextures;
	};
	struct MODEL_INSTANCES // each instance of a model in the level
	{
		unsigned modelIndex, transformStart, transformCount, flags; // flags optional
	};
	struct MATERIAL_TEXTURES // swaps string pointers for loaded texture offsets
	{
		unsigned int albedoIndex, roughnessIndex, metalIndex, normalIndex;
	};
	struct BLENDER_OBJECT // *NEW* Used to track individual objects in blender
	{
		const char* blendername; // *NEW* name of model straight from blender (FLECS)
		unsigned int modelIndex, transformIndex;
	};
	// All geometry data combined for level to be loaded onto the video card
	std::vector<H2B::VERTEX> levelVertices;
	std::vector<unsigned> levelIndices;
	// All material data used by the level
	std::vector<H2B::MATERIAL> levelMaterials;
	// This could be populated by the Level_Renderer during GPU transfer
	std::vector<MATERIAL_TEXTURES> levelTextures; // same size as LevelMaterials
	// All transform data used by each model
	std::vector<GW::MATH::GMATRIXF> levelTransforms;
	// *NEW* All level boundry data used by the models
	std::vector<GW::MATH::GOBBF> levelColliders;
	// All required drawing information combined
	std::vector<H2B::BATCH> levelBatches;
	std::vector<H2B::MESH> levelMeshes;
	std::vector<LEVEL_MODEL> levelModels;
	// what we actually draw once loaded (using GPU instancing)
	std::vector<MODEL_INSTANCES> levelInstances;
	// *NEW* each item from the blender scene graph
	std::vector<BLENDER_OBJECT> blenderObjects;

	// Material Attributes
	//std::vector<H2B::ATTRIBUTES> Attributes;
	std::vector<H2B::MATERIAL> Materials;

	//LIGHT TRIAL
	std::vector<POINT_LIGHT> levelPointLights;
	std::vector<SPOT_LIGHT> levelSpotLights;

	// Imports the default level txt format and collects all .h2b data
	bool LoadLevel(const char* gameLevelPath,
		const char* h2bFolderPath,
		GW::SYSTEM::GLog log) {
		// What this does:
		// Parse GameLevel.txt 
		// For each model found in the file...
			// if not encountered create new unique temporary model entry.
				// Add model transform to a list of transforms for this model.(instances)
			// if already encountered, just add its transfrom to the existing model entry.
		// when finished, traverse model entries to import each model's data to the class.
		std::set<MODEL_ENTRY> uniqueModels; // unique models and their locations
		log.LogCategorized("EVENT", "LOADING GAME LEVEL [DATA ORIENTED]");

		UnloadLevel();// clear previous level data if there is any
		if (ReadGameLevel(gameLevelPath, uniqueModels, log) == false) {
			log.LogCategorized("ERROR", "Fatal error reading game level, aborting level load.");
			return false;
		}
		if (ReadAndCombineH2Bs(h2bFolderPath, uniqueModels, log) == false) {
			log.LogCategorized("ERROR", "Fatal error combining H2B mesh data, aborting level load.");
			return false;
		}
		// Added the materials to a vector
		if (levelMaterials.size() != 0)
		{
			for (size_t i = 0; i < levelMaterials.size(); i++)
			{
				//Attributes.push_back(levelMaterials[i].attrib);
				Materials.push_back(levelMaterials[i]);
			}
		}
		// level loaded into CPU ram
		log.LogCategorized("EVENT", "GAME LEVEL WAS LOADED TO CPU [DATA ORIENTED]");
		return true;
	}
	// used to wipe CPU level data between levels
	void UnloadLevel() {
		level_strings.clear();
		levelVertices.clear();
		levelIndices.clear();
		levelMaterials.clear();
		levelTextures.clear();
		levelBatches.clear();
		levelMeshes.clear();
		levelModels.clear();
		levelTransforms.clear();
		levelInstances.clear();
		blenderObjects.clear();
		//Attributes.clear();
		Materials.clear();
		levelPointLights.clear();
		levelSpotLights.clear();
	}
	// *NO RENDERING/GPU/DRAW LOGIC IN HERE PLEASE* 
	// *DATA ORIENTED SHOULD AIM TO SEPERATE DATA FROM THE LOGIC THAT USES IT*
	// The Level Renderer class is a good place to utilize this data.
	// You can use your chosen API to have one GPU buffer for each type of data.
	// Then you loop through instances using the API features to draw each mesh only once.
private:
	// internal defintion for reading the GameLevel layout 
	struct MODEL_ENTRY
	{
		std::string modelFile; // path to .h2b file
		// *NEW* object aligned bounding box data: // LBN, LTN, LTF, LBF, RBN, RTN, RTF, RBF
		GW::MATH2D::GVECTOR3F boundry[8];
		mutable std::vector<std::string> blenderNames; // *NEW* names from blender
		mutable std::vector<GW::MATH::GMATRIXF> instances; // where to draw
		bool operator<(const MODEL_ENTRY& cmp) const {
			return modelFile < cmp.modelFile; // you need this for std::set to work
		}
		// *NEW* converts the vec3 boundries to an OBB
		GW::MATH::GOBBF ComputeOBB(GW::MATH::GVECTORF scale, GW::MATH::GVECTORF pos) const {
			GW::MATH::GOBBF out = {
				GW::MATH::GIdentityVectorF,
				GW::MATH::GIdentityVectorF,
				GW::MATH::GIdentityQuaternionF // initally unrotated (local space)
			};
			out.center.x = (boundry[0].x + boundry[4].x) * 0.5f + pos.x;
			out.center.y = (boundry[0].y + boundry[1].y) * 0.5f + pos.y;
			out.center.z = (boundry[0].z + boundry[2].z) * 0.5f + pos.z;
			out.extent.x = std::fabsf(boundry[0].x - boundry[4].x) * 0.5f * (scale.x * 0.5);
			out.extent.y = std::fabsf(boundry[0].y - boundry[1].y) * 0.5f * (scale.y * 0.5);
			out.extent.z = std::fabsf(boundry[0].z - boundry[2].z) * 0.5f * (scale.z * 0.5);
			return out;
		}
	};
	// internal helper for reading the game level
	bool ReadGameLevel(const char* gameLevelPath,
		std::set<MODEL_ENTRY>& outModels,
		GW::SYSTEM::GLog log) {
		log.LogCategorized("MESSAGE", "Begin Reading Game Level Text File.");
		GW::SYSTEM::GFile file;
		bool InstancedModel =false;
		file.Create();
		if (-file.OpenTextRead(gameLevelPath)) {
			log.LogCategorized(
				"ERROR", (std::string("Game level not found: ") + gameLevelPath).c_str());
			return false;
		}
		char linebuffer[1024];
		while (+file.ReadLine(linebuffer, 1024, '\n'))
		{
			// having to have this is a bug, need to have Read/ReadLine return failure at EOF
			if (linebuffer[0] == '\0')
				break;
			if (std::strcmp(linebuffer, "MESH") == 0)
			{
				std::string blenderName;
				file.ReadLine(linebuffer, 1024, '\n');
				if (std::strcmp(linebuffer, "INSTANCED") == 0)
				{
					InstancedModel = true;

					file.ReadLine(linebuffer, 1024, '\n');
					blenderName = linebuffer;
				}
				else
				{
					blenderName = linebuffer;
					log.LogCategorized("INFO", (std::string("Model Detected: ") + blenderName).c_str());

				}
				//file.ReadLine(linebuffer, 1024, '\n');

				// create the model file name from this (strip the .001)
				MODEL_ENTRY add = { linebuffer };
				add.modelFile = add.modelFile.substr(0, add.modelFile.find_last_of("."));
				add.modelFile += ".h2b";

				// now read the transform data as we will need that regardless
				GW::MATH::GMATRIXF transform;
				for (int i = 0; i < 4; ++i) {
					file.ReadLine(linebuffer, 1024, '\n');
					// read floats
					sscanf_s(linebuffer + 13, "%f, %f, %f, %f",
						&transform.data[0 + i * 4], &transform.data[1 + i * 4],
						&transform.data[2 + i * 4], &transform.data[3 + i * 4]);
				}
				std::string loc = "Location: X ";
				loc += std::to_string(transform.row4.x) + " Y " +
					std::to_string(transform.row4.y) + " Z " + std::to_string(transform.row4.z);
				log.LogCategorized("INFO", loc.c_str());

				// *NEW* finally read in the boundry data for this model
				bool colliderFound = false;
				unsigned int currPos;
				file.Seek(-1, 0, currPos);
				for (int i = 0; i < 8; ++i) {
					file.ReadLine(linebuffer, 1024, '\n');
					bool test = std::string(linebuffer).find("Vector") != std::string::npos;
					if (test) {
						// read floats
						std::sscanf(linebuffer + 9, "%f, %f, %f",
							&add.boundry[i].x, &add.boundry[i].y, &add.boundry[i].z);
						colliderFound = true;
					}
					else {
						file.Seek(currPos, 0, currPos);
						break;
					}
				}
				if (colliderFound) {
					std::string bounds = "Boundry: Left ";
					bounds += std::to_string(add.boundry[0].x) +
						" Right " + std::to_string(add.boundry[4].x) +
						" Bottom " + std::to_string(add.boundry[0].y) +
						" Top " + std::to_string(add.boundry[1].y) +
						" Near " + std::to_string(add.boundry[0].z) +
						" Far " + std::to_string(add.boundry[2].z);
					log.LogCategorized("INFO", bounds.c_str());
				}

				// does this model already exist?
				auto found = outModels.find(add);
				if (found == outModels.end()) // no
				{
					add.blenderNames.push_back(blenderName); // *NEW*
					if (InstancedModel)
					{
						for (size_t i = 0; i < 50; i++)
						{

							add.instances.push_back(transform);
						}
					}
					else
					{
						add.instances.push_back(transform);

					}
					outModels.insert(add);
				}
				else // yes
				{
					found->blenderNames.push_back(blenderName); // *NEW*
					found->instances.push_back(transform);
				}
			}
			// If Light
			else if (std::strcmp(linebuffer, "LIGHT") == 0)
			{
				file.ReadLine(linebuffer, 1024, '\n');
				log.LogCategorized("INFO", (std::string("Light Detected: ") + linebuffer).c_str());

				GW::MATH::GMATRIXF transform;
				for (int i = 0; i < 4; ++i) {
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer + 13, "%f, %f, %f, %f",
						&transform.data[0 + i * 4], &transform.data[1 + i * 4],
						&transform.data[2 + i * 4], &transform.data[3 + i * 4]);
				}

				file.ReadLine(linebuffer, 1024, '\n');
				// If POINT
				if (std::strcmp(linebuffer, "TYPE: POINT") == 0)
				{
					// Create light and push to vector
					POINT_LIGHT light;
					light.transform = transform;
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer + 10, "%f, g=%f, b=%f", &light.color.x, &light.color.y, &light.color.z);
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer, "%f", &light.watts);
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer, "%f", &light.cutoff_distance);
					levelPointLights.push_back(light);
				}

				// If SPOT
				else if (std::strcmp(linebuffer, "TYPE: SPOT") == 0)
				{
					// Create spotlight and push to vector
					SPOT_LIGHT light;
					light.transform = transform;
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer + 10, "%f, g=%f, b=%f", &light.color.x, &light.color.y, &light.color.z);
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer, "%f", &light.watts);
					file.ReadLine(linebuffer, 1024, '\n');
					sscanf_s(linebuffer, "%f", &light.cutoff_distance);

					levelSpotLights.push_back(light);
				}

				std::string loc = "Location: X ";
				loc += std::to_string(transform.row4.x) + " Y " +
					std::to_string(transform.row4.y) + " Z " + std::to_string(transform.row4.z);
				log.LogCategorized("INFO", loc.c_str());

			}
		}
		log.LogCategorized("MESSAGE", "Game Level File Reading Complete.");
		return true;
	}
	// internal helper for collecting all .h2b data into unified arrays
	bool ReadAndCombineH2Bs(const char* h2bFolderPath,
		const std::set<MODEL_ENTRY>& modelSet,
		GW::SYSTEM::GLog log) {
		log.LogCategorized("MESSAGE", "Begin Importing .H2B File Data.");
		// parse each model adding to overall arrays
		H2B::Parser p; // reads the .h2b format
		const std::string modelPath = h2bFolderPath;
		for (auto i = modelSet.begin(); i != modelSet.end(); ++i)
		{
			if (p.Parse((modelPath + "/" + i->modelFile).c_str()))
			{
				log.LogCategorized("INFO", (std::string("H2B Imported: ") + i->modelFile).c_str());
				// transfer all string data
				LEVEL_MODEL model;
				
				for (int j = 0; j < p.materialCount; ++j) {
					H2B::MATERIAL mat = p.materials[j];
					/*std::set<std::string>::iterator loc = level_strings.find(mat.name);
					if (loc != level_strings.end()) {*/
						if (mat.name != "") level_strings.insert(mat.name);
						//assign diffuse
						if (mat.map_Kd != "") {
							level_strings.insert(mat.map_Kd);
							model.useTextures = true;
						}
						//assign specular
						if (mat.map_Ks != "") {
							level_strings.insert(mat.map_Ks);
						}
						//assign ambient
						if (mat.map_Ka != "") {
							level_strings.insert(mat.map_Ka);
						}
						//assign normal
						if (mat.map_Ns != "") {
							level_strings.insert(mat.map_Ns);
						}
						levelMaterials.push_back(mat);
					/*}
					else {

					}*/
					/*for (int k = 0; k < 10; ++k) {
						if (*((&p.materials[j].name) + k) != "")
							*((&p.materials[j].name) + k) =
							level_strings.insert(*((&p.materials[j].name) + k)).first->c_str();
					}*/
				}
				for (int j = 0; j < p.meshCount; ++j) {
					if (p.meshes[j].name != nullptr)
						p.meshes[j].name =
						level_strings.insert(p.meshes[j].name).first->c_str();
				}
				// record source file name & sizes
				model.filename = level_strings.insert(i->modelFile).first->c_str();
				model.vertexCount = p.vertexCount;
				model.indexCount = p.indexCount;
				model.materialCount = p.materialCount;
				model.meshCount = p.meshCount;
				// record offsets
				model.vertexStart = levelVertices.size();
				model.indexStart = levelIndices.size();
				model.materialStart = levelMaterials.size() - p.materialCount;
				model.batchStart = levelBatches.size();
				model.meshStart = levelMeshes.size();
				// append/move all data
				levelVertices.insert(levelVertices.end(), p.vertices.begin(), p.vertices.end());
				levelIndices.insert(levelIndices.end(), p.indices.begin(), p.indices.end());
				levelMaterials.insert(levelMaterials.end(), p.materials.begin(), p.materials.end());
				levelBatches.insert(levelBatches.end(), p.batches.begin(), p.batches.end());
				levelMeshes.insert(levelMeshes.end(), p.meshes.begin(), p.meshes.end());
				// *NEW* add overall collision volume(OBB) for this model and it's submeshes 
				model.colliderIndex = levelColliders.size();
				// add level model
				levelModels.push_back(model);
				// add level model instances
				MODEL_INSTANCES instances;
				instances.flags = 0; // shadows? transparency? much we could do with this.
				instances.modelIndex = levelModels.size() - 1;
				instances.transformStart = levelTransforms.size();
				instances.transformCount = i->instances.size();
				levelTransforms.insert(levelTransforms.end(), i->instances.begin(), i->instances.end());
				GW::MATH::GVECTORF scale;
				GW::MATH::GMatrix::GetScaleF(levelTransforms[levelTransforms.size() - 1], scale);
				GW::MATH::GVECTORF v = levelTransforms[levelTransforms.size() - 1].row4;
				levelColliders.push_back(i->ComputeOBB(scale, v));
				// add instance set
				levelInstances.push_back(instances);
				// *NEW* Add an entry for each unique blender object
				int offset = 0;
				for (auto& n : i->blenderNames) {
					BLENDER_OBJECT obj{
						level_strings.insert(n).first->c_str(),
						instances.modelIndex, instances.transformStart + offset++
					};
					blenderObjects.push_back(obj);
				}
			}
			else {
				// notify user that a model file is missing but continue loading
				log.LogCategorized("ERROR",
					(std::string("H2B Not Found: ") + modelPath + "/" + i->modelFile).c_str());
				log.LogCategorized("WARNING", "Loading will continue but model(s) are missing.");
			}
		}
		log.LogCategorized("MESSAGE", "Importing of .H2B File Data Complete.");
		return true;
	}
};