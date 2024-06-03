#pragma once
#include "vkCore.h"

#include "vkEngine.h"
#include "vkDevice.h"

namespace vk
{
    class vkEngine;
    class vkDevice;

    class vkTexture {
    public:
        vkTexture(const vkEngine* pEngine);

        ~vkTexture();

        bool LoadFromBuffer(const unsigned char* buffer, const VkDeviceSize& bufferSize, VkFormat imageFormat, uint32_t texWidth, uint32_t texHeight,
            VkFilter filter = VK_FILTER_LINEAR, VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Getters for Vulkan objects
        const VkImage GetImage() const { return mImage; }
        const VkImageView GetImageView() const { return mImageView; }
        const VkSampler GetSampler() const { return mSampler; }
        const VkDeviceMemory GetDeviceMemory() const { return mImageMemory; }
        const VkDescriptorImageInfo GetDescriptorImageInfo() {
            mDescriptorInfo = { mSampler, mImageView, mImageLayout };
            return mDescriptorInfo; }

    private:
        const vkEngine* m_pEngine;
        std::string mName;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mMipLevels;
        uint32_t mComponent;
        uint32_t mBitsPerChannel;
        VkFormat mImageFormat;
        VkImageLayout mImageLayout;
        VkImageUsageFlags mUsageFlags;

        VkImage mImage;
        VkDeviceMemory mImageMemory;
        VkImageView mImageView;
        VkSampler mSampler;
        VkDescriptorImageInfo mDescriptorInfo;

        friend class vkResourceLoader;
    };

}