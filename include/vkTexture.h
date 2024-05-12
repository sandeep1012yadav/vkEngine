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
        vkTexture(const vkEngine* pEngine) : m_pEngine(pEngine), 
            mImage(VK_NULL_HANDLE), mImageMemory(VK_NULL_HANDLE), mImageView(VK_NULL_HANDLE), mSampler(VK_NULL_HANDLE),
            mName(""), mWidth(0), mHeight(0), mComponent(4), mBitsPerChannel(8), mImageFormat(VK_FORMAT_UNDEFINED), mImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
            mUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT), mMipLevels(0)
        {}

        ~vkTexture() 
        {
            const vkDevice* pDevice = m_pEngine->GetDevice();
            if (mSampler != VK_NULL_HANDLE) {
                vkDestroySampler(pDevice->GetLogicalDevice(), mSampler, nullptr);
            }
            if (mImageView != VK_NULL_HANDLE) {
                vkDestroyImageView(pDevice->GetLogicalDevice(), mImageView, nullptr);
            }
            if (mImage != VK_NULL_HANDLE) {
                vkDestroyImage(pDevice->GetLogicalDevice(), mImage, nullptr);
            }
            if (mImageMemory != VK_NULL_HANDLE) {
                vkFreeMemory(pDevice->GetLogicalDevice(), mImageMemory, nullptr);
            }
        }

        // Method to load a texture from a file (stubbed for this example)
        bool LoadFromFile(const std::string& filePath) {
            // Load the image data using a library like stb_image
            // Create a VkImage and allocate memory for it
            // Create an image view and a sampler
            // Upload the image data to the GPU
            // Transition the image layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            // Don't forget to clean up any staging resources you create
            return true;
        }

        bool LoadFromBuffer(const unsigned char* buffer, const VkDeviceSize& bufferSize, VkFormat imageFormat, uint32_t texWidth, uint32_t texHeight, 
            VkFilter filter = VK_FILTER_LINEAR, VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            bool bStatus = true;
            vkDevice* pDevice = m_pEngine->GetDevice();

            mImageFormat = imageFormat;
            mWidth = texWidth;
            mHeight = texHeight;
            mMipLevels = 1;
            mUsageFlags = imageUsageFlags;
            mImageLayout = imageLayout;

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            bStatus |= pDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory);

            // Map memory and copy vertex data
            void* data;
            vkMapMemory(pDevice->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, buffer, (size_t)bufferSize);
            vkUnmapMemory(pDevice->GetLogicalDevice(), stagingBufferMemory);

            VkExtent3D imageExtent3D = { texWidth, texHeight, 1 };
            mUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            bStatus |= pDevice->CreateImage(VK_IMAGE_TYPE_2D, imageFormat, imageExtent3D, mMipLevels, 1, VK_SAMPLE_COUNT_1_BIT,
                VK_IMAGE_TILING_OPTIMAL, mUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage, mImageMemory);

            
            VkCommandBuffer copyCmd = pDevice->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
            
            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresourceRange.baseMipLevel = 0;
            subresourceRange.levelCount = mMipLevels;
            subresourceRange.layerCount = 1;
            vk::tools::ChangeImageLayout(copyCmd, mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            VkBufferImageCopy bufferCopyRegion = {};
            bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.mipLevel = 0;
            bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
            bufferCopyRegion.imageSubresource.layerCount = 1;
            bufferCopyRegion.imageOffset = { 0,0,0 };
            bufferCopyRegion.imageExtent = { texWidth, texHeight, 1 };// Depth is 1 for 2D textures 
            
            bufferCopyRegion.bufferOffset = 0;
            bufferCopyRegion.bufferRowLength = 0; // Tightly packed
            bufferCopyRegion.bufferImageHeight = 0; // Tightly packed

            vkCmdCopyBufferToImage(
                copyCmd, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // The image is in the transfer destination layout
                1, // Copying one region
                &bufferCopyRegion
            );

            // Continue recording commands as needed...
            vk::tools::ChangeImageLayout(copyCmd, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, subresourceRange,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            pDevice->FlushCommandBuffer(copyCmd, pDevice->GetGraphicsQueue(), true);

            VkImageViewCreateInfo imageViewCI = vk::initializers::ImageViewCreateInfo(mImage, VK_IMAGE_VIEW_TYPE_2D, mImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

            VkResult result = vkCreateImageView(pDevice->GetLogicalDevice(), &imageViewCI, nullptr, &mImageView);
            if (result != VK_SUCCESS) {
                vkLog->Log("vkTexture : Image creation failed...");
                bStatus = false;
            }

            return bStatus;
        }

        // Getters for Vulkan objects
        const VkImage GetImage() const { return mImage; }
        const VkImageView GetImageView() const { return mImageView; }
        const VkSampler GetSampler() const { return mSampler; }

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

        // Helper methods for creating image, image view, and sampler would go here
        // For example:
        // void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        // void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
        // void CreateSampler(VkSampler& sampler);
        // void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        // void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


        friend class vkResourceLoader;
    };

}