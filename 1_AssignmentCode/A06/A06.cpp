// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"


// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec4 color;
};


// MAIN ! 
class A06 : public BaseProject {
	protected:

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	Model<Vertex> M1;
	// Descriptor sets
	DescriptorSet DS1;
	// Textures
	Texture T1;

	// Other application parameters
	int currScene = 0;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A06 - Compile the shaders";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 4;
		texturesInPool = 4;
		setsInPool = 4;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSL.init(this, {
					{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV},
				  {0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color),
				         sizeof(glm::vec4), COLOR}
				});

		// Pipelines [Shader couples]
		P.init(this, &VD, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSL});
		P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);
		// Models, textures and Descriptors (values assigned to the uniforms)
		M1.vertices = {{{-1.0,-1.0,0.9f}, {0.0f,0.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{-1.0, 1.0,0.9f}, {0.0f,1.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{ 1.0,-1.0,0.9f}, {1.0f,0.0f}, {1.0f,1.0f,1.0f,0.83f}},
					   {{ 1.0, 1.0,0.9f}, {1.0f,1.0f}, {1.0f,1.0f,1.0f,0.83f}}};
		M1.indices = {0, 1, 2,    1, 3, 2};
		M1.initMesh(this, &VD);
		
		// Create the textures
		T1.init(this,   "textures/Checker.png");

		// Init local variables
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		DS1.init(this, &DSL, {
					{0, TEXTURE, 0, &T1}
				});
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		// Cleanup datasets
		DS1.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		T1.cleanup();
		
		// Cleanup models
		M1.cleanup();
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);

		// binds the data set
		DS1.bind(commandBuffer, P, 0, currentImage);
					
		// binds the model
		M1.bind(commandBuffer);
		
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				currScene = (currScene+1) % 2;
				std::cout << "Scene : " << currScene << "\n";

				if(!screenshotSaved) {
					saveScreenshot("A06.png", currentImage);
				}

				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}		
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A06 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}