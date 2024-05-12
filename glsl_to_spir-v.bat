cd C:\MyWorkspace\VulkanEngine\vkEngine
"%VULKAN_SDK%\Bin\glslc.exe" .\resources\shaders\triangle\default.vert -o .\resources\shaders\triangle\default_vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" .\resources\shaders\triangle\default.frag -o .\resources\shaders\triangle\default_frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" .\resources\shaders\standard\standard.vert -o .\resources\shaders\standard\standard_vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" .\resources\shaders\standard\standard.frag -o .\resources\shaders\standard\standard_frag.spv