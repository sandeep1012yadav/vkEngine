#include "vkSamplerManager.h"
#include "vkTexture.h"

namespace vk
{
    vkTexture::vkTexture(const vkEngine* pEngine) : m_pEngine(pEngine),
        mImage(VK_NULL_HANDLE), mImageMemory(VK_NULL_HANDLE), mImageView(VK_NULL_HANDLE), mSampler(VK_NULL_HANDLE),
        mName(""), mWidth(0), mHeight(0), mComponent(4), mBitsPerChannel(8), mImageFormat(VK_FORMAT_UNDEFINED), mImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
        mUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT), mMipLevels(0)
    {}

    vkTexture::~vkTexture()
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

    bool vkTexture::LoadFromBuffer(const unsigned char* buffer, const VkDeviceSize& bufferSize, VkFormat imageFormat, uint32_t texWidth, uint32_t texHeight,
        VkFilter filter, VkImageUsageFlags imageUsageFlags, VkImageLayout imageLayout)
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

        // Define a common sampler
        VkSamplerCreateInfo commonSamplerInfo = {};
        commonSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        commonSamplerInfo.magFilter = VK_FILTER_LINEAR;
        commonSamplerInfo.minFilter = VK_FILTER_LINEAR;
        commonSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        commonSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        commonSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        commonSamplerInfo.anisotropyEnable = VK_TRUE;
        commonSamplerInfo.maxAnisotropy = 16.0f;
        commonSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        commonSamplerInfo.unnormalizedCoordinates = VK_FALSE;
        commonSamplerInfo.compareEnable = VK_FALSE;
        commonSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        commonSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        commonSamplerInfo.mipLodBias = 0.0f;
        commonSamplerInfo.minLod = 0.0f;
        commonSamplerInfo.maxLod = VK_LOD_CLAMP_NONE;

        mSampler = vkSamplerManager::GetInstance()->GetSampler(commonSamplerInfo);

        return bStatus;
    }
}