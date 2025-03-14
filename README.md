# :trophy: Polimi-COMPUTER GRAPHICS-Vulkan-ExamSummary-090958-(GRIBAUDO MARCO)
- Some guides and summaries for Polimi's computer science's COMPUTER GRAPHICS(Vulkan). The course is 090958 - COMPUTER GRAPHICS (GRIBAUDO MARCO) [2023-24] from Politecnico di Milano - polimi.  
- Thanks to **Professor GRIBAUDO MARCO**.  
- "This guide was created by AchesonDan@163.com based on his own learning process, for study and reference only."  
- This repository provides a comprehensive guide to the **COMPUTER GRAPHICS** exam, including **1_AssignmentCode, 2_Vulkan_Project, 3_HowtodeliverAssignment, and 4_InitialVulkanSource, 5_ScreenshotOfProjects.**.

# :video_game: About the Game project
- In this game, you can move in the first person perspective and help Batman find the enemy Joker in a dark environment (a parking lot with multiple cars).  
- You use a flashlight and an AK47 gun. The flashlight has multiple colors and reflects different effects in different scenes.  
- When you find the clown, press 0 to capture the clown and teleport to the next scene, the rocket Launch station, where you will ride the rocket away and the entire game is over.  
- **About the project, you can directly see this PDF file.**[CG-The Dark Knight's Quest for Justice.pdf](2_Vulkan_Project/CG-The%20Dark%20Knight's%20Quest%20for%20Justice.pdf)

# :pizza: My voice!
- My exam date was the 14th of January in 2025. I have been preparing for this exam from the 14th of November to the 26th of December. **(43 days, Half of each day)**
- I wrote this repository beginning on the 7th of February, 2025, and ending on the 14th of March. **(7 days)**
- I scored 30 points in this course, 8 points for assignments, and 22+ points for the project.
- Because of the file size limit, 3 models, Joker2.obj, Spaceshuttle.obj, and vulkan initial source(which you need to download from https://vulkan-tutorial.com/Development_environment) were deleted.

# :coffee: Some Tips?
1. In **1_AssignmentCode**, it includes all the assignments I did. The name is Dan_A0N, and A0N is the initial version from the professor.
2. In **2_Vulkan_Project**, includes a pdf to introduce my project - 'CG-The Dark Knight's Quest for Justice', I use 5 different shaders to fulfill different Illumination effect, reflection effect, illumination color...
3. In **3_HowtodeliverAssignment**, They are the deliver files.
4. I downloaded the OBJ and texture from https://rigmodels.com/index.php#google_vignette. It is a good website to download the OBJs you want and which you can use in our vulkan and project.
5. For the obj you downloaded, you need to check the code of obj if it has vn, **if the code of obj doesn't have VN, we can not use it in our vulkan project.**
6. In **5_ScreenshotOfProjects**, I uploaded the screenshots from my project.
7. When you modify or run shaders, **remember to complie GLSL source code (e.g., Shader.frag and Shader.vert) into its corresponding SPIR-V binary (e.g., ShaderFrag.spv and ShaderVert.spv) in your teminal,** Spi-v is an intermediate language primarily used by modern graphics apis such as Vulkan to load and use these shader files at runtime.
```bash
glslangValidator -V Shader.vert -o ShaderVert.spv
glslangValidator -V Shader.frag -o ShaderFrag.spv
```

# :pushpin: Contents
1. [1_AssignmentCode](1_AssignmentCode)
2. [2_Vulkan_Project](2_Vulkan_Project)
3. [3_HowtodeliverAssignment](3_HowtodeliverAssignment)
4. [4_InitialVulkanSource](4_InitialVulkanSource)
5. [5_ScreenshotOfProjects](5_ScreenshotOfProjects)

## Contributing
Feel free to submit pull requests or open issues to suggest improvements or share additional exercises!
