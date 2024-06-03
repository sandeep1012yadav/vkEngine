#include "vkCore.h"
#include "vkTexture.h"
#include "vkMesh.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkMaterial.h"
#include "vkResourcePool.h"
#include "vkResourceLoader.h"


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"

namespace vk
{
	vkResourceLoader* vkResourceLoader::m_pResourceLoader = nullptr;

	vkResourceLoader* vkResourceLoader::GetInstance()
	{
		if (m_pResourceLoader == nullptr) {
			vkLog->Log("vkResourceLoader : vkEngine is not initailized..");
		}
		return m_pResourceLoader;
	}

	vkResourceLoader* vkResourceLoader::CreateInstance(const vkEngine* pEngine)
	{
		if (m_pResourceLoader == nullptr)
		{
			m_pResourceLoader = new vkResourceLoader(pEngine);
		}
		
		return m_pResourceLoader;
	}

	vkResourceLoader::vkResourceLoader(const vkEngine* pEngine) : m_pEngine(pEngine)
	{

	}

	vkResourceLoader::~vkResourceLoader()
	{

	}

	vkFrameObject* vkResourceLoader::LoadGLTFNode(const tinygltf::Node& node, const tinygltf::Model& model, const std::map<uint32_t, uint32_t>& materialIdMap,
		vkFrameObject* pParentFrameObject, vkGameObject* pParentGameObject)
	{
		vkFrameObject* pFrameObject = new vkFrameObject(node.name);
		// Get the local node matrix
		// It's either made up from translation, rotation, scale or a 4x4 matrix
		if (node.translation.size() == 3) {
			pFrameObject->mLocalTransformation = glm::translate(pFrameObject->mLocalTransformation, glm::vec3(glm::make_vec3(node.translation.data())));
		}
		if (node.rotation.size() == 4) {
			glm::quat q = glm::make_quat(node.rotation.data());
			pFrameObject->mLocalTransformation *= glm::mat4(q);
		}
		if (node.scale.size() == 3) {
			pFrameObject->mLocalTransformation = glm::scale(pFrameObject->mLocalTransformation, glm::vec3(glm::make_vec3(node.scale.data())));
		}
		if (node.matrix.size() == 16) {
			pFrameObject->mLocalTransformation = glm::make_mat4x4(node.matrix.data());
		};

		pFrameObject->m_pParentGameObject = pParentGameObject;
		pFrameObject->m_pParent = pParentFrameObject;

		

		// Load node's children
		uint32_t nbNodeChildren = static_cast<uint32_t>(node.children.size());
		pFrameObject->mNbChildren = nbNodeChildren;
		if (nbNodeChildren > 0)
		{
			pFrameObject->m_pChildren = new vkFrameObject*[nbNodeChildren];
			for (size_t i = 0; i < nbNodeChildren; i++)
			{
				pFrameObject->m_pChildren[i] = LoadGLTFNode(model.nodes[node.children[i]], model, materialIdMap, pFrameObject, pParentGameObject);
			}
		}

		std::vector<uint32_t>& vIndices = pParentGameObject->m_vIndices;
		std::vector<vkVertex>& vVertices = pParentGameObject->m_vVertices;

		if (node.mesh > -1)
		{
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			vkMesh* pMesh = new vkMesh(mesh.name);
			
			// Iterate through all primitives of this node's mesh
			for (size_t i = 0; i < mesh.primitives.size(); i++) 
			{
				const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
				uint32_t firstIndex = static_cast<uint32_t>(vIndices.size());
				uint32_t vertexStart = static_cast<uint32_t>(vVertices.size());
				uint32_t indexCount = 0;
				// Vertices
				{
					const float* positionBuffer = nullptr;
					const float* normalsBuffer = nullptr;
					const float* texCoordsBuffer = nullptr;
					size_t vertexCount = 0;

					// Get buffer data for vertex positions
					if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) 
					{
						const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("POSITION")->second];
						const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
						positionBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
						vertexCount = accessor.count;
					}
					// Get buffer data for vertex normals
					if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
						const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
						normalsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}
					// Get buffer data for vertex texture coordinates
					// glTF supports multiple sets, we only load the first one
					if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
						const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
						const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
						texCoordsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}

					// Append data to model's vertex buffer
					for (size_t v = 0; v < vertexCount; v++) {
						vkVertex vert{};
						vert.vPos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
						vert.vNormal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
						vert.vTexCoord = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
						vert.vColor = glm::vec3(1.0f);
						vVertices.push_back(vert);
					}
				}
				// Indices
				{
					const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.indices];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					indexCount += static_cast<uint32_t>(accessor.count);

					// glTF supports different component types of indices
					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							vIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							vIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							vIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					default:
						vkLog->Log("Index component type ", accessor.componentType, " not supported!");
						return nullptr;
					}
				}
				vkPrimitive primitive{};
				primitive.mStartIndex = firstIndex;
				primitive.mNbIndices = indexCount;
				primitive.mMaterialIndex = glTFPrimitive.material >= 0 ? materialIdMap.at(glTFPrimitive.material) : -1;
				pMesh->AddPrimitive(primitive);

				// incrementing sampler count
				pParentGameObject->mNbMaterialDescriptorSets++;
			}
			pFrameObject->AssignMesh(pMesh);
		}
		return pFrameObject;
	}

	void vkResourceLoader::LoadGLTFMaterials(const tinygltf::Model& model, const std::map<uint32_t, uint32_t>& textureIdMap, std::map<uint32_t, uint32_t>& materialIdMap)
	{
		// Lambda function to convert tinygltf::Value::Array to glm::vec4
		auto ConvertToVec4 = [](const tinygltf::Value::Array& array) -> glm::vec4 {
			return glm::vec4(
				static_cast<float>(array[0].Get<double>()),
				static_cast<float>(array[1].Get<double>()),
				static_cast<float>(array[2].Get<double>()),
				static_cast<float>(array[3].Get<double>())
			);
		};

		auto ConvertToVec3 = [](const tinygltf::Value::Array& array) -> glm::vec3 {
			return glm::vec3(
				static_cast<float>(array[0].Get<double>()),
				static_cast<float>(array[1].Get<double>()),
				static_cast<float>(array[2].Get<double>())
			);
		};

		//loading materials
		for (uint32_t i = 0; i < model.materials.size(); i++) 
		{
			tinygltf::Material glTFMaterial = model.materials[i];

			vkMaterial* pMaterial = new vkMaterial(m_pEngine, glTFMaterial.name);

			// Get the base color factor
			if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
				pMaterial->mDiffuseFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
			}

			// Get base color texture index
			if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) 
			{
				int gltfTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
				if (gltfTextureIndex >= 0 && gltfTextureIndex < model.textures.size())
				{
					const tinygltf::Texture& texture = model.textures[gltfTextureIndex];
					int gltfImageIndex = texture.source;

					if (gltfImageIndex >= 0 && gltfImageIndex < model.images.size())
					{
						pMaterial->mDiffuseTextureIndex = textureIdMap.at(gltfImageIndex);
					}
				}
			}

			// Check for extensions
			auto extIt = glTFMaterial.extensions.find("KHR_materials_pbrSpecularGlossiness");
			if (extIt != glTFMaterial.extensions.end()) 
			{
				const auto& extension = extIt->second;
				// Access the extension data
				if (extension.Has("diffuseTexture")) 
				{
					const auto& diffuseTexture = extension.Get("diffuseTexture");
					if (diffuseTexture.Has("index")) 
					{
						int diffuseTextureIndex = diffuseTexture.Get("index").Get<int>();
						if (diffuseTextureIndex >= 0 && diffuseTextureIndex < model.textures.size()) 
						{
							const tinygltf::Texture& texture = model.textures[diffuseTextureIndex];
							int imageIndex = texture.source;
							if (imageIndex >= 0 && imageIndex < model.images.size()) 
							{
								pMaterial->mDiffuseTextureIndex = textureIdMap.at(imageIndex);
							}
						}
					}
				}
				if (extension.Has("diffuseFactor"))
				{
					const auto& diffuseFactor = extension.Get("diffuseFactor");
					pMaterial->mDiffuseFactor = ConvertToVec4(diffuseFactor.Get<tinygltf::Value::Array>());
				}

				if (extension.Has("specularGlossinessTexture"))
				{
					const auto& specularGlossinessTexture = extension.Get("specularGlossinessTexture");
					if (specularGlossinessTexture.Has("index"))
					{
						int specularTextureIndex = specularGlossinessTexture.Get("index").Get<int>();
						if (specularTextureIndex >= 0 && specularTextureIndex < model.textures.size())
						{
							const tinygltf::Texture& texture = model.textures[specularTextureIndex];
							int imageIndex = texture.source;
							if (imageIndex >= 0 && imageIndex < model.images.size())
							{
								pMaterial->mSpecularTextureIndex = textureIdMap.at(imageIndex);
							}
						}
					}
				}
				if (extension.Has("specularFactor"))
				{
					const auto& specularFactor = extension.Get("specularFactor");
					pMaterial->mSpecularFactor = ConvertToVec3(specularFactor.Get<tinygltf::Value::Array>());
				}
				if (extension.Has("glossinessFactor"))
				{
					pMaterial->mGlossinessFactor = (float)extension.Get("glossinessFactor").Get<double>();
				}
			}

			pMaterial->mNormalTextureIndex = glTFMaterial.normalTexture.index >= 0 ? textureIdMap.at(glTFMaterial.normalTexture.index) : -1;
			pMaterial->mOcculsionTextureIndex = glTFMaterial.occlusionTexture.index >= 0 ? textureIdMap.at(glTFMaterial.occlusionTexture.index) : -1;
			pMaterial->mEmissiveTextureIndex = glTFMaterial.emissiveTexture.index >= 0 ? textureIdMap.at(glTFMaterial.emissiveTexture.index) : -1;
			pMaterial->mEmissiveFactor = glm::vec3((float)glTFMaterial.emissiveFactor[0], (float)glTFMaterial.emissiveFactor[1], (float)glTFMaterial.emissiveFactor[2]);
			pMaterial->bDoubleSided = glTFMaterial.doubleSided;

			uint32_t matId = vkResourcePool::GetInstance()->AddMaterial(pMaterial);
			materialIdMap[i] = matId;
		}
	}
	void vkResourceLoader::LoadGLTFTextures(const tinygltf::Model& model, std::map<uint32_t, uint32_t>& textureIdMap)
	{
		// loading textures
		for (uint32_t i = 0; i < model.images.size(); i++)
		{
			const tinygltf::Image& gltfImage = model.images[i];

			vkTexture* pTexture = new vkTexture(m_pEngine);
			pTexture->mName = gltfImage.name;
			pTexture->mComponent = gltfImage.component;

			// Get the image data from the glTF loader
			unsigned char* buffer = nullptr;
			VkDeviceSize bufferSize = 0;
			bool deleteBuffer = false;
			// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
			if (gltfImage.component == 3)
			{
				bufferSize = gltfImage.width * gltfImage.height * 4;
				buffer = new unsigned char[bufferSize];
				unsigned char* rgba = buffer;
				const unsigned char* rgb = &gltfImage.image[0];
				for (size_t i = 0; i < gltfImage.width * gltfImage.height; ++i)
				{
					memcpy(rgba, rgb, sizeof(unsigned char) * 3);
					rgba += 4;
					rgb += 3;
				}
				deleteBuffer = true;
			}
			else
			{
				buffer = (unsigned char*) & gltfImage.image[0];
				bufferSize = gltfImage.image.size();
			}
			// Load texture from image buffer
			pTexture->LoadFromBuffer(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, gltfImage.width, gltfImage.height);
			if (deleteBuffer) {
				delete[] buffer;
			}

			uint32_t textureId = vkResourcePool::GetInstance()->AddTexture(pTexture);

			textureIdMap[i] = textureId;
		}
	}

	vkGameObject* vkResourceLoader::LoadGameObjectFromGLTF(const std::string fileName)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		std::string error, warning;


		bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, fileName);

		std::map<uint32_t, uint32_t> textureIdMap;
		LoadGLTFTextures(gltfModel, textureIdMap);
		std::map<uint32_t, uint32_t> materialIdMap;
		LoadGLTFMaterials(gltfModel, textureIdMap, materialIdMap);

		if (fileLoaded)
		{
			const tinygltf::Scene& scene = gltfModel.scenes[0];
			for (size_t i = 0; i < scene.nodes.size(); i++)
			{
				const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];

				vkGameObject* pGameObj = new vkGameObject();

				pGameObj->m_pFrameObject = LoadGLTFNode(node, gltfModel, materialIdMap, nullptr, pGameObj);

				return pGameObj;
			}
		}
		return nullptr;
	}
}