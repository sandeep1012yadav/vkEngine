#include "vkEngine.h"
#include "vkDevice.h"
#include "vkSamplerManager.h"

namespace vk
{
    // private member functions
    vkSamplerManager* vkSamplerManager::m_pSamplerManager = nullptr;
    vkSamplerManager* vkSamplerManager::GetInstance()
    {
        return m_pSamplerManager;
    }

    void vkSamplerManager::CreateInstance(const vkEngine* pEngine)
    {
        m_pSamplerManager = new vkSamplerManager(pEngine);
    }

    void vkSamplerManager::DestroyInstance()
    {
        delete m_pSamplerManager;
    }

    size_t vkSamplerManager::HashCombine(size_t lhs, size_t rhs)
    {
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        return lhs;
    }

    size_t vkSamplerManager::HashSamplerCreateInfo(const VkSamplerCreateInfo& samplerInfo)
    {
        size_t hash = 0;

        // Hash individual fields
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.magFilter));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.minFilter));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.mipmapMode));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.addressModeU));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.addressModeV));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.addressModeW));
        hash = HashCombine(hash, std::hash<float>()(samplerInfo.mipLodBias));
        hash = HashCombine(hash, std::hash<VkBool32>()(samplerInfo.anisotropyEnable));
        hash = HashCombine(hash, std::hash<float>()(samplerInfo.maxAnisotropy));
        hash = HashCombine(hash, std::hash<VkBool32>()(samplerInfo.compareEnable));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.compareOp));
        hash = HashCombine(hash, std::hash<float>()(samplerInfo.minLod));
        hash = HashCombine(hash, std::hash<float>()(samplerInfo.maxLod));
        hash = HashCombine(hash, std::hash<int>()(samplerInfo.borderColor));
        hash = HashCombine(hash, std::hash<VkBool32>()(samplerInfo.unnormalizedCoordinates));

        return hash;
    }

    vkSamplerManager::vkSamplerManager(const vkEngine* pEngine) : m_pEngine(pEngine)
    {
        m_pDevice = m_pEngine->GetDevice();
    }

    // public member functions
    vkSamplerManager::~vkSamplerManager()
    {
        for (auto& pair : m_Samplers)
        {
            vkDestroySampler(m_pDevice->GetLogicalDevice(), pair.second, nullptr);
        }
    }

    VkSampler vkSamplerManager::GetSampler(const VkSamplerCreateInfo& samplerInfo)
    {
        // Generate a hash for the samplerInfo
        size_t hash = HashSamplerCreateInfo(samplerInfo);

        // Check if the sampler already exists
        auto it = m_Samplers.find(hash);
        if (it != m_Samplers.end()) {
            return it->second;
        }

        // Create a new sampler
        VkSampler sampler;
        VkResult result = vkCreateSampler(m_pDevice->GetLogicalDevice(), &samplerInfo, nullptr, &sampler);
        if (result != VK_SUCCESS) {
            vkLog->Log("vkSamplerManager : Failed to create sampler...");
        }

        // Store and return the new sampler
        m_Samplers[hash] = sampler;
        return sampler;
    }
}